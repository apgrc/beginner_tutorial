#include <libserial/SerialStream.h>
#include <string>
#include <sstream>
#include <iostream>
#include <thread>
#include <iomanip>

#include "ros/ros.h"
#include "beginner_tutorials/ps3.h"

#define maxOut 7199
#define k 9
#define L1_DIGITAL 4
#define R1_DIGITAL 5
#define R2_ANALOG 5
#define R2_DIGITAL 7
#define LEFT_STICK_HORIZONTAL 3
/*
 * Delay Function
 *
 * include <chrono>
 * include <thread>
 * std::this_thread::sleep_for(std::chrono::milliseconds(50));
 */
using namespace std;

enum com:uint8_t {
    Hello = 0, Start, Continue, Stop, Sensor_Start, Sensor_Stop, Reset
};

#pragma pack(2)
struct order{
    com command;
    // uint16_t out_m0;
    // uint16_t out_m1;
    int32_t out_m0;
    int32_t out_m1;
  };

#pragma pack(2)
struct receive{
    _Float32 time;
    __int32_t position1;
    __int32_t position2;
    uint16_t distance[5];
};

LibSerial::SerialStream serial_stream;

void receive_candy(LibSerial::SerialStream& stream) {
    receive candy;
    while (stream.IsOpen()) {
        if (stream.IsDataAvailable()) {
            receive candy;
            stream.read((char *) (&candy), sizeof(candy));
            for (int i = 0; i < sizeof(candy.distance) / sizeof(candy.distance[0]); i++) {
                cout << setw(10) << candy.distance[i];
            }
            cout << endl;
        }
    }
}

// void ps3Callback(const  beginner_tutorials::ps3::ConstPtr& msg)
void ps3Callback(const  beginner_tutorials::ps3::ConstPtr& msg)
{
  // beginner_tutorials::ps3& data = msg;
  order out;
  out.command = Continue;
  int32_t tmp = (msg->axis[R2_ANALOG] + 32767) / k;  
  if (msg->buttons[L1_DIGITAL])
    tmp = tmp * -1;

  cout << "input = " << tmp << endl;
  out.out_m0 = tmp;
  out.out_m1 = tmp;
  if(msg->axis[LEFT_STICK_HORIZONTAL] > 0)
    out.out_m0 -= msg->axis[LEFT_STICK_HORIZONTAL] / k;
  else 
    out.out_m1 += msg->axis[LEFT_STICK_HORIZONTAL] / k;

  serial_stream.write(reinterpret_cast<const char *>(&out), sizeof(out));
  // cout << "Sended " << tmp << endl;
  cout << " axis: ";
  for (size_t i(0); i < msg->axis.size(); ++i)
      cout << " " << setw(5) << msg->axis[i];
  cout << endl;

  cout << "  button: ";
  for (size_t i(0); i < msg->buttons.size(); ++i)
      cout << " " << (int) msg->buttons[i];
  cout << endl;
}

int main(int argc, char *argv[]) {

  ros::init(argc, argv, "ps3_listener");
  ros::NodeHandle n;
  ros::Subscriber sub = n.subscribe("ps3_controller", 1000, ps3Callback);

    serial_stream.Open("/dev/ttyACM0");
    serial_stream.SetBaudRate(LibSerial::BaudRate::BAUD_115200);

    std::thread t(receive_candy, std::ref(serial_stream));
    cout << "connected!" << endl;


    // int num = 0;
    // order my_data;
    // string output;
    // string input;
    // while (input[0] != 'z') {
        // input.clear();
        // output.clear();
        // cout << "0: Hello" << endl;
        // cout << "1: Start" << endl;
        // cout << "2: Continue" << endl;
        // cout << "3: Stop" << endl;
        // cout << "4: Start Sensor" << endl;
        // cout << "5: Stop Sensor" << endl;
        // cout << "Input command: ";
        // getline(cin, input);
        // if (input[0] == 'z')
            // break;
        // stringstream(input) >> num;
        // my_data.command = static_cast<com>(num);
//
        // if (my_data.command == Sensor_Start)
        // {
            // serial_stream.write(reinterpret_cast<const char *>(&my_data), sizeof(my_data));
            // continue;
        // }
//
        // if (my_data.command == Sensor_Stop)
        // {
            // serial_stream.write(reinterpret_cast<const char *>(&my_data), sizeof(my_data));
            // continue;
        // }
        // if (num > 0 && num < 3) {
            // cout << "Motor 0 (max 7199): ";
            // getline(cin, input);
            // stringstream(input) >> num;
            // my_data.out_m0 = static_cast<int16_t>(num);
//
            // cout << "Motor 1 (max 7199): ";
            // getline(cin, input);
            // stringstream(input) >> num;
            // my_data.out_m1 = static_cast<int16_t>(num);
        // } else {
            // my_data.out_m0 = 0;
            // my_data.out_m1 = 0;
        // }
        // serial_stream.write(reinterpret_cast<const char *>(&my_data), sizeof(my_data));
    // }
    ros::spin();
    serial_stream.Close();
    t.join();
    return 0;
}
