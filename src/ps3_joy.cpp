#include <iostream>
#include <iomanip>
#include <vector>
#include <cstdio>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/joystick.h>
#include <typeinfo>
#include <algorithm>    // std::copy
#include <thread>

#include "ros/ros.h"
#include "beginner_tutorials/ps3.h"

#include <sstream>

#define JOY_DEV "/dev/input/js0"

using namespace std;


vector<char> joy_button;
vector<int> joy_axis;

void ros_pub(ros::Publisher& ps3_pub){
    ros::Rate r(10);
    beginner_tutorials::ps3 msg;
    while (ros::ok()) {
        msg.axis = joy_axis;
        vector<signed char> tmp(joy_button.begin(),joy_button.end());
        msg.buttons = tmp;
        ps3_pub.publish(msg);

        ros::spinOnce();
        // ros::spin();
        r.sleep();
    }
}

int main(int argc, char **argv) {

    ros::init(argc, argv, "ps3_pub");
    ros::NodeHandle n;
    ros::Publisher ps3_pub= n.advertise<beginner_tutorials::ps3>("ps3_controller", 50);
    int joy_fd(-1), num_of_axis(0), num_of_buttons(0);
    char name_of_joystick[80];

    if ((joy_fd = open(JOY_DEV, O_RDONLY)) < 0) {
        cerr << "Failed to open " << JOY_DEV << endl;
        return -1;
    }

    ioctl(joy_fd, JSIOCGAXES, &num_of_axis);
    ioctl(joy_fd, JSIOCGBUTTONS, &num_of_buttons);
    ioctl(joy_fd, JSIOCGNAME(80), &name_of_joystick);

    joy_button.resize(num_of_buttons, 0);
    joy_axis.resize(num_of_axis, 0);

    cout << "Joystick: " << name_of_joystick << endl
         << "  axis: " << num_of_axis << endl
         << "  buttons: " << num_of_buttons << endl;

    fcntl(joy_fd, F_SETFL, O_NONBLOCK);   // using non-blocking mode

    std::thread t(ros_pub,std::ref(ps3_pub));

    while (ros::ok()) {

        js_event js;

        read(joy_fd, &js, sizeof(js_event));

        switch (js.type & ~JS_EVENT_INIT) {
            case JS_EVENT_AXIS:
                if ((int) js.number >= joy_axis.size()) {
                    cerr << "err:" << (int) js.number << endl;
                    continue;
                }
                joy_axis[(int) js.number] = js.value;
                break;
            case JS_EVENT_BUTTON:
                if ((int) js.number >= joy_button.size()) {
                    cerr << "err:" << (int) js.number << endl;
                    continue;
                }
                joy_button[(int) js.number] = js.value;
                break;
        }

        cout << " axis: ";
        for (size_t i(0); i < joy_axis.size(); ++i)
            cout << " " << setw(5) << joy_axis[i];
        cout << endl;

        cout << "  button: ";
        for (size_t i(0); i < joy_button.size(); ++i)
            cout << " " << (int) joy_button[i];
        cout << endl;

       usleep(100);
    }

    close(joy_fd);
    t.join();
    return 0;
}
