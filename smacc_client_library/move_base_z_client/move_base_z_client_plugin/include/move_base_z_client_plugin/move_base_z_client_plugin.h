/*****************************************************************************************************************
 * ReelRobotix Inc. - Software License Agreement      Copyright (c) 2018
 * 	 Authors: Pablo Inigo Blasco, Brett Aldrich
 *
 ******************************************************************************************************************/
#pragma once

#include <smacc/smacc.h>
#include <smacc/client_bases/smacc_action_client_base.h>

#include <move_base_msgs/MoveBaseAction.h>
#include <planner_switcher/planner_switcher.h>

namespace smacc
{
class WaypointNavigator;

class ClMoveBaseZ : public SmaccActionClientBase<move_base_msgs::MoveBaseAction>
{
    typedef SmaccActionClientBase<move_base_msgs::MoveBaseAction> Base;

public:
    typedef SmaccActionClientBase<move_base_msgs::MoveBaseAction>::ResultConstPtr ResultConstPtr;

    std::shared_ptr<planner_switcher::PlannerSwitcher> plannerSwitcher_;

    std::shared_ptr<smacc::WaypointNavigator> waypointsNavigator_;

    ClMoveBaseZ();

    virtual void initialize() override;

    virtual std::string getName() const override;

    virtual ~ClMoveBaseZ();

    template <typename TDerived, typename TObjectTag>
    void assignToOrthogonal();

private:
    std::function<void(int index)> postWayPointEvent;
};

} // namespace smacc

#include <move_base_z_client_plugin/impl/move_base_z_client_plugin_impl.h>