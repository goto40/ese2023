/******************************************************************************
 * Copyright 2023 Pierre Bayerl and Michael Thoma
 * This program and the accompanying materials are made available under the
 * terms of the MIT License, which is available in the project root.
 ******************************************************************************/
#include <chrono>
#include <cstdlib>
#include <iostream>
#include <string>
#include <thread>

#include "mqtt/async_client.h"

int main(int argc, char* argv[]) {
  using namespace std::chrono_literals;

  std::string STATUS_TOPIC = "ese/mqtt_publisher/status";

  try {
    std::string address = (argc > 1) ? std::string(argv[1]) : "broker",
                clientID = (argc > 2) ? std::string(argv[2]) : "mqtt_publisher";
    mqtt::async_client client(address, clientID);

    auto connOpts =
        mqtt::connect_options_builder()
            .clean_session()  // see:
                              // https://www.youtube.com/watch?v=2ETj1fM7-ZA&list=PLRkdoPznE1EMXLW6XoYLGd4uUaB6wB0wd&index=8
            .will(mqtt::message(
                STATUS_TOPIC, "offline", 1,
                true))  // see:
                        // https://www.youtube.com/watch?v=dNy9GEXngoE&list=PLRkdoPznE1EMXLW6XoYLGd4uUaB6wB0wd&index=10
            .keep_alive_interval(
                1s)  // see
                     // https://www.youtube.com/watch?v=2EsrWOFPmc4&list=PLRkdoPznE1EMXLW6XoYLGd4uUaB6wB0wd&index=11
            .finalize();

    std::cout << "connect...\n";
    client.connect(connOpts)->wait();
    std::cout << "connected.\n";

    mqtt::topic data_topic(client, "ese/mqtt_publisher/data", 1);

    size_t cnt = 0;
    while (true) {
      if (cnt % 100 == 0) {
        std::cout << "sending status...\n";
        mqtt::message_ptr pubmsg = mqtt::make_message(STATUS_TOPIC, "online");
        mqtt::delivery_token_ptr pubtok = client.publish(pubmsg);
        pubtok->wait();
      }
      cnt++;

      // also send data:
      data_topic.publish(std::to_string(rand() % 100));

      std::this_thread::sleep_for(100ms);
    }

    std::cout << "end.\n";
  } catch (std::exception& e) {
    std::cerr << e.what() << "\n";
  }
}
