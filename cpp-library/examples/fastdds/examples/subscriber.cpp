/******************************************************************************
 * Copyright 2023 Pierre Bayerl and Michael Thoma
 * This program and the accompanying materials are made available under the
 * terms of the MIT License, which is available in the project root.
 ******************************************************************************/
#include <fastrtps/types/TypesBase.h>

#include <atomic>
#include <cassert>
#include <chrono>
#include <fastdds/dds/core/status/SubscriptionMatchedStatus.hpp>
#include <fastdds/dds/domain/DomainParticipant.hpp>
#include <fastdds/dds/domain/DomainParticipantFactory.hpp>
#include <fastdds/dds/subscriber/DataReader.hpp>
#include <fastdds/dds/subscriber/DataReaderListener.hpp>
#include <fastdds/dds/subscriber/SampleInfo.hpp>
#include <fastdds/dds/subscriber/Subscriber.hpp>
#include <fastdds/dds/subscriber/qos/DataReaderQos.hpp>
#include <fastdds/dds/subscriber/qos/SubscriberQos.hpp>
#include <fastdds/dds/topic/TypeSupport.hpp>

#include "dds_examples/HelloWorld.h"
#include "dds_examples/HelloWorldPubSubTypes.h"

// see
// https://fast-dds.docs.eprosima.com/en/latest/fastdds/getting_started/simple_app/simple_app.html#write-the-fast-dds-publisher
namespace dds_examples {

struct SubListener : eprosima::fastdds::dds::DataReaderListener {
  std::atomic_flag connected = false;
  HelloWorldT
      data;  // probbaly not thread safe - put only into queue or similar!

  void on_subscription_matched(
      eprosima::fastdds::dds::DataReader *,
      const eprosima::fastdds::dds::SubscriptionMatchedStatus &status)
      override {
    if (status.current_count > 0)
      (void)connected.test_and_set();
    else
      connected.clear();
    std::cout << (connected.test() ? "connected" : "diconnected")
              << " current_count=" << status.current_count << "\n";
  }
  void on_data_available(eprosima::fastdds::dds::DataReader *reader) override {
    eprosima::fastdds::dds::SampleInfo info;
    if (reader->take_next_sample(&data, &info) ==
        eprosima::fastrtps::types::ReturnCode_t::RETCODE_OK) {
      if (info.valid_data) {
        std::cout << "received '" << data.message() << "' with index "
                  << data.index() << "\n";
      }
    }
  }
};

struct MySubscriber {
  eprosima::fastdds::dds::DomainParticipant *domainParticipant;
  eprosima::fastdds::dds::Subscriber *subscriber;
  eprosima::fastdds::dds::Topic *topic;
  eprosima::fastdds::dds::DataReader *dataReader;
  eprosima::fastdds::dds::TypeSupport typeSupport{new HelloWorldTPubSubType{}};
  SubListener subListener;
  eprosima::fastdds::dds::DomainId_t domain_id = 0x0;

  ~MySubscriber() {
    if (subscriber && dataReader) {
      subscriber->delete_datareader(dataReader);
    }
    if (domainParticipant && topic) {
      domainParticipant->delete_topic(topic);
    }
    if (domainParticipant && subscriber) {
      domainParticipant->delete_subscriber(subscriber);
    }
    if (domainParticipant) {
      eprosima::fastdds::dds::DomainParticipantFactory::get_instance()
          ->delete_participant(domainParticipant);
    }
  }

  void init() {
    eprosima::fastdds::dds::DomainParticipantQos domainParticipantQos;
    domainParticipantQos.name("MySubscriber");
    domainParticipantQos.wire_protocol()
        .builtin.discovery_config.leaseDuration =
        eprosima::fastrtps::Duration_t(5, 20);

    domainParticipant =
        eprosima::fastdds::dds::DomainParticipantFactory::get_instance()
            ->create_participant(domain_id, domainParticipantQos);
    assert(domainParticipant != nullptr);
    typeSupport.register_type(domainParticipant);

    topic = domainParticipant->create_topic(
        "Ese-Hello-World-Topic", "HelloWorld",
        eprosima::fastdds::dds::TOPIC_QOS_DEFAULT);
    assert(topic != nullptr);

    subscriber = domainParticipant->create_subscriber(
        eprosima::fastdds::dds::SUBSCRIBER_QOS_DEFAULT, nullptr);
    assert(subscriber != nullptr);

    dataReader = subscriber->create_datareader(
        topic, eprosima::fastdds::dds::DATAREADER_QOS_DEFAULT, &subListener);
    assert(dataReader != nullptr);
  }

  void run() {
    while (true) {
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
  }
};
}  // namespace dds_examples

int main(int, char **) {
  std::cout << "Starting subscriber." << std::endl;
  dds_examples::MySubscriber mysub;
  mysub.init();
  std::cout << "Running subscriber." << std::endl;
  mysub.run();
}
