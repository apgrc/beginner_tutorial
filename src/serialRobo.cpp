#include <libserial/SerialStream.h>
#include <string>
#include <sstream>
#include <iostream>
#include <thread>
#include <iomanip>

#include "ros/ros.h"
#include "loggerhead_bot/ps3.h"

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

// void ps3Callback(const  loggerhead_bot::ps3::ConstPtr& msg)
void ps3Callback(const loggerhead_bot::ps3::ConstPtr& msg)
{
  order out;
  out.command = Continue;
  int32_t tmp = (msg->axis[R2_ANALOG] + 32767) / k;  
  if (msg->buttons[L1_DIGITAL])
    tmp = tmp * -1;

  // cout << "input = " << tmp << endl;
  out.out_m0 = tmp;
  out.out_m1 = tmp;
  if(msg->axis[LEFT_STICK_HORIZONTAL] > 0)
    out.out_m0 -= msg->axis[LEFT_STICK_HORIZONTAL] / k;
  else 
    out.out_m1 += msg->axis[LEFT_STICK_HORIZONTAL] / k;

  serial_stream.write(reinterpret_cast<const char *>(&out), sizeof(out));
  // cout << "Sended " << tmp << endl;
  // cout << " axis: ";
  // for (size_t i(0); i < msg->axis.size(); ++i)
    // cout << " " << setw(5) << msg->axis[i];
  // cout << endl;
//
  // cout << "  button: ";
  // for (size_t i(0); i < msg->buttons.size(); ++i)
    // cout << " " << (int) msg->buttons[i];
  // cout << endl;
}

void topi_menu(LibSerial::SerialStream& serial_stream) {
  int num = 0;
  string input;
  order topi_order;
  while (ros::ok()) {
    input.clear();
    cout << "4: start" << endl;
    cout << "5: break (stop)" << endl;
    cout << "6: move some steps" << endl;
    // cout << "R: reset position" << endl;
    cout << "Input command: ";
    getline(cin, input);
    // input << num;
    // serial_stream << input[0];
    switch (input[0]) {
      case '4':
        topi_order.command = Sensor_Start;
        serial_stream.write(reinterpret_cast<const char *>(&topi_order), sizeof(topi_order));
        //TODO
        break;
      case '5':
        topi_order.command = Sensor_Stop;
        serial_stream.write(reinterpret_cast<const char *>(&topi_order), sizeof(topi_order));
        //TODO
        break;
      case '6':
        topi_order.command = Reset;
        serial_stream.write(reinterpret_cast<const char *>(&topi_order), sizeof(topi_order));
        //TODO
        break;
    }
  }
}
int main(int argc, char *argv[]) {

  ros::init(argc, argv, "ps3_listener");
  ros::NodeHandle n;
  ros::Subscriber sub = n.subscribe("ps3_controller", 1000, ps3Callback);

  serial_stream.Open("/dev/ttyACM0");
  serial_stream.SetBaudRate(LibSerial::BaudRate::BAUD_115200);
  cout << "connected!" << endl;

  std::thread t(receive_candy, std::ref(serial_stream));
  std::thread t2(topi_menu, std::ref(serial_stream));

  ros::spin();
  serial_stream.Close();
  t.join();
  t2.join();
  return 0;
}
