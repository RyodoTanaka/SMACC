#pragma once
#include <smacc/smacc_orthogonal.h>
#include <ros_timer_client/cl_ros_timer.h>

namespace sm_dance_bot
{
class OrTimer : public smacc::Orthogonal
{
public:
    virtual void onInitialize() override
    {
        auto actionclient = this->createClient<OrTimer, ros_timer_client::ClRosTimer>(ros::Duration(0.5));
        actionclient->initialize();
    }
};
}