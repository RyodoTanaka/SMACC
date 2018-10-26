#pragma once
#include "plugins/move_base_to_goal.h"
#include "plugins/reel.h"
#include "radial_motion.h"

#include <non_rt_helper/DispenseModeGoal.h>

namespace NavigateToRadialStart 
{
using namespace smacc;

//forward declarations of subcomponents of this state
struct NavigationOrthogonalLine;
struct ReelOrthogonalLine;
struct Navigate;
struct ReelStartAndDispense;

//--------------------------------------------
/// State NavigateToRadialStart
struct NavigateToRadialStart
    : SmaccState<NavigateToRadialStart, RadialMotionStateMachine,
                 mpl::list<NavigationOrthogonalLine, ReelOrthogonalLine>> // <- these are the orthogonal lines of this State
{
  // when this state is finished then move to the RotateDegress state
  typedef sc::transition<EvStateFinished, RotateDegress::RotateDegress>
      reactions;

public:
  // This is the state constructor. This code will be executed when the
  // workflow enters in this substate (that is according to statechart the moment when this object is created)
  // after this, its orthogonal lines are created (see orthogonal line classes).
  NavigateToRadialStart(my_context ctx)
      : SmaccState<NavigateToRadialStart, RadialMotionStateMachine,
                   mpl::list<NavigationOrthogonalLine, ReelOrthogonalLine>>(ctx) 
    {
       ROS_INFO("Entering in NavigateToRadialStart State");
    }

  // This is the state destructor. This code will be executed when the
  // workflow exits from this state (that is according to statechart the moment when this object is destroyed)
  ~NavigateToRadialStart() 
  {
    ROS_INFO("Finishing NavigateToRadialStart state");
  }
};

//--------------------------------------------------
// orthogonal line 0
struct NavigationOrthogonalLine
    : public SmaccState<NavigationOrthogonalLine,
                        NavigateToRadialStart::orthogonal<0>, Navigate> 
  {
  // This is the orthogonal line constructor. This code will be executed when the
  // workflow enters in this orthogonal line (that is according to statechart the moment when this object is created)
  NavigationOrthogonalLine(my_context ctx)
      : SmaccState<NavigationOrthogonalLine,
                   NavigateToRadialStart::orthogonal<0>, Navigate>(ctx) // call the SmaccState base constructor
  {
    ROS_INFO("Entering in move_base orthogonal line");
  }

  // This is the state destructor. This code will be executed when the
  // workflow exits from this state (that is according to statechart the moment when this object is destroyed)
  ~NavigationOrthogonalLine() 
  {
    ROS_INFO("Finishing move base orthogonal line");
  }
};

//--------------------------------------------------
// this is the navigate substate inside the navigation orthogonal line of the NavigateToRadialStart State
struct Navigate : SmaccState<Navigate, NavigationOrthogonalLine> {
  typedef mpl::list<sc::custom_reaction<EvActionClientSuccess>,
                    sc::custom_reaction<EvReelInitialized>> reactions;

public:
  // This is the substate constructor. This code will be executed when the
  // workflow enters in this substate (that is according to statechart the moment when this object is created)
  Navigate(my_context ctx) 
    : SmaccState<Navigate, NavigationOrthogonalLine>(ctx) // call the SmaccState base constructor 
  {
    ROS_INFO("Entering Navigate");

    // this substate will need access to the "MoveBase" resource or plugin. In this line
    // you get the reference to this resource.
    moveBaseClient_ =
        context<RadialMotionStateMachine>()
            .requiresActionClient<smacc::SmaccMoveBaseActionClient>(
                "move_base");
  }

  // auxiliar function that defines the motion that is requested to the move_base action server
  void goToRadialStart() {
    smacc::SmaccMoveBaseActionClient::Goal goal;
    goal.target_pose.header.frame_id = "/odom";
    goal.target_pose.header.stamp = ros::Time::now();

    goal.target_pose.pose.position.x = 3;
    goal.target_pose.pose.position.y = 0;
    goal.target_pose.pose.orientation.w = 1;
    context<RadialMotionStateMachine>().setData("radial_start_pose",
                                                goal.target_pose);

    moveBaseClient_->sendGoal(goal);
  }

  // when the reel substate is finished we will react starting the motion
  sc::result react(const EvReelInitialized &ev) 
  { 
    goToRadialStart(); 
  }

  // this is the callback when the navigate action of this state is finished
  // if it succeeded we will notify to the parent State to finish sending a EvStateFinishedEvent
  sc::result react(const EvActionClientSuccess &ev) {

    if (ev.client == moveBaseClient_) {
      if (ev.getResult() == actionlib::SimpleClientGoalState::SUCCEEDED) {
        ROS_INFO("move base, goal position reached");

        // we have finished the motion, notify this is the end of the state
        post_event(EvStateFinished());
        return forward_event(); // this->terminate();
      } 
      else if (ev.getResult() == actionlib::SimpleClientGoalState::ABORTED) 
      {
        // repeat the navigate action request to the move base node if we get ABORT as response
        // It may work if try again. Move base sometime rejects the request because it is busy.
        goToRadialStart();

        // this event was for us. We have used it without moving to any other state. Do not let others consume it.
        return discard_event();
      }
    } 
    else 
    {
      // the action client event success is not for this substate. Let others process this event.
      ROS_INFO("navigate substate lets other process the EvActionClientSuccessEv");
      return forward_event();
    }
  }

  // This is the substate destructor. This code will be executed when the
  // workflow exits from this substate (that is according to statechart the moment when this object is destroyed)
  ~Navigate() { ROS_INFO("Exiting move goal Action Client"); }

private:
  // keeps the reference to the move_base resorce or plugin (to connect to the move_base action server). 
  // this resource can be used from any method in this state
  smacc::SmaccMoveBaseActionClient *moveBaseClient_;
};

//--------------------------------------------------
// orthogonal line 1
// this is the reel substate inside the reel orthogonal line of the NavigateToRadialStart State
struct ReelOrthogonalLine
    : SmaccState<ReelOrthogonalLine, NavigateToRadialStart::orthogonal<1>,
                 ReelStartAndDispense> {
public:
  // This is the orthogonal line constructor. This code will be executed when the
  // workflow enters in this orthogonal line (that is according to statechart the moment when this object is created)
  ReelOrthogonalLine(my_context ctx)
      : SmaccState<ReelOrthogonalLine, NavigateToRadialStart::orthogonal<1>,
                   ReelStartAndDispense>(ctx) // call the SmaccState base constructor                 
  {
    ROS_INFO("Entering in reel orthogonal line");
  }

  ~ReelOrthogonalLine() { ROS_INFO("Finishing reel orthogonal line"); }
};

//--------------------------------------------------
struct ReelStartAndDispense
    : SmaccState<ReelStartAndDispense, ReelOrthogonalLine> {
  typedef sc::custom_reaction<EvActionClientSuccess> reactions;

public:

  // This is the substate constructor. This code will be executed when the
  // workflow enters in this substate (that is according to statechart the moment when this object is created)
  ReelStartAndDispense(my_context ctx) 
    : SmaccState<ReelStartAndDispense, ReelOrthogonalLine>(ctx) // call the SmaccState base constructor
  {
    ROS_INFO("Entering ReelStartAndDispense");

    // this substate will need access to the "Reel" resource or plugin. In this line
    // you get the reference to this resource.
    reelActionClient_ = context<RadialMotionStateMachine>()
                            .requiresActionClient<smacc::SmaccReelActionClient>(
                                "non_rt_helper");

    dispense();
  }

  void dispense()
  {
    // use the reel resource to request dispense (request to the non_rt_helper)
    smacc::SmaccReelActionClient::Goal goal;
    goal.dispense_mode = smacc::SmaccReelActionClient::Goal::DISPENSE;
    reelActionClient_->sendGoal(goal);
  }

  sc::result react(const EvActionClientSuccess &ev) {

    // if the reel request is finished and success, then notify the event to the move base substate
    // and finish this substate
    if (ev.client == reelActionClient_ ) 
    {
      if( ev.getResult() == actionlib::SimpleClientGoalState::SUCCEEDED)
      {
        ROS_INFO("Received event for reel client");
        // notify the navigate substate that we finished
        post_event(EvReelInitialized());

        return terminate();
      } 
      else 
      {
        // try again
        ROS_INFO("Retry reel dispense");
        dispense();
        
        // consume
        return discard_event();
      }
    } 
    else 
    {
      // the action client event success is not for this substate. Let others process this event.
      ROS_INFO("reel substate lets other process the EvActionClientSuccessEv");
      return forward_event();
    }
  }

  // This is the substate destructor. This code will be executed when the
  // workflow exits from this substate (that is according to statechart the moment when this object is destroyed)
  ~ReelStartAndDispense() { ROS_INFO("Exiting Reel_Action Client"); }

private:
  // keeps the reference to the reel resorce or plugin (to connect to the non_rt_helper)
  smacc::SmaccReelActionClient *reelActionClient_;
};
}
