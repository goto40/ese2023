/******************************************************************************
 * Copyright 2023 Pierre Bayerl and Michael Thoma
 * This program and the accompanying materials are made available under the
 * terms of the MIT License, which is available in the project root.
 ******************************************************************************/
#include <fastdds/rtps/common/Time_t.h>
#include <fastrtps/types/TypesBase.h>

#include <atomic>
#include <cassert>
#include <chrono>
#include <fastdds/dds/core/status/PublicationMatchedStatus.hpp>
#include <fastdds/dds/domain/DomainParticipant.hpp>
#include <fastdds/dds/domain/DomainParticipantFactory.hpp>
#include <fastdds/dds/domain/qos/DomainParticipantQos.hpp>
#include <fastdds/dds/publisher/DataWriter.hpp>
#include <fastdds/dds/publisher/DataWriterListener.hpp>
#include <fastdds/dds/publisher/Publisher.hpp>
#include <fastdds/dds/publisher/qos/DataWriterQos.hpp>
#include <fastdds/dds/publisher/qos/PublisherQos.hpp>
#include <fastdds/dds/topic/Topic.hpp>
#include <fastdds/dds/topic/TypeSupport.hpp>
#include <fastdds/dds/topic/qos/TopicQos.hpp>
#include <thread>

#include "dds_examples/HelloWorld.h"
#include "dds_examples/HelloWorldPubSubTypes.h"

// see
// https://fast-dds.docs.eprosima.com/en/latest/fastdds/getting_started/simple_app/simple_app.html#write-the-fast-dds-publisher
namespace dds_examples {

struct PubListener : eprosima::fastdds::dds::DataWriterListener {
  std::atomic_flag connected = false;
  void on_publication_matched(
      eprosima::fastdds::dds::DataWriter *,
      const eprosima::fastdds::dds::PublicationMatchedStatus &status) override {
    if (status.current_count > 0)
      (void)connected.test_and_set();
    else
      connected.clear();
    std::cout << (connected.test() ? "connected" : "diconnected")
              << " current_count=" << status.current_count << "\n";
  }
};

struct MyPublisher {
  dds_examples::HelloWorldT data;
  eprosima::fastdds::dds::DomainParticipant *domainParticipant;
  eprosima::fastdds::dds::Publisher *publisher;
  eprosima::fastdds::dds::Topic *topic;
  eprosima::fastdds::dds::DataWriter *dataWriter;
  eprosima::fastdds::dds::TypeSupport typeSupport{
      new dds_examples::HelloWorldTPubSubType{}};
  PubListener pubListener;
  eprosima::fastdds::dds::DomainId_t domain_id = 0x0;

  void init() {
    data.index(0);
    data.message("Hello ESE");
    data.color(ColorT::BLUE);
    data.id(99);
    data.values()[0] = 11;
    data.values()[1] = 22;
    data.values()[2] = 33;
    data.colors().resize(4);
    data.colors().at(0) = ColorT::RED;
    data.colors().at(1) = ColorT::GREEN;
    data.colors().at(2) = ColorT::BLUE;
    data.colors().at(3) = ColorT::RED;

    eprosima::fastdds::dds::DomainParticipantQos domainParticipantQos;
    domainParticipantQos.name("MyPublisher");
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

    publisher = domainParticipant->create_publisher(
        eprosima::fastdds::dds::PUBLISHER_QOS_DEFAULT, nullptr);
    assert(publisher != nullptr);

    dataWriter = publisher->create_datawriter(
        topic, eprosima::fastdds::dds::DATAWRITER_QOS_DEFAULT, &pubListener);
    assert(dataWriter != nullptr);
  }

  bool publish() {
    if (pubListener.connected.test()) {
      data.index(data.index() + 1);
      dataWriter->write(&data);
      return true;
    } else {
      return false;
    }
  }

  void run() {
    size_t dataSent = 0;
    while (true) {
      if (publish()) {
        dataSent++;
        std::cout << "send " << data.index() << "\n";
      }
      std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
  }
};
}  // namespace dds_examples

int main(int, char **) {
  std::cout << "Starting publisher." << std::endl;
  dds_examples::MyPublisher mypub;
  mypub.init();
  std::cout << "Running publisher." << std::endl;
  mypub.run();
}
