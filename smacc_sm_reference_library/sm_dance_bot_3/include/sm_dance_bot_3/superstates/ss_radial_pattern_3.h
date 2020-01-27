#include <smacc/smacc.h>

namespace sm_dance_bot_3
{
namespace SS3
{

namespace sm_dance_bot_3
{
namespace radial_motion_states
{
//forward declaration for initial ssr
class StiRadialRotate;
class StiRadialReturn;
class StiRadialEndPoint;
class StiRadialLoopStart;
} // namespace radial_motion_states
} // namespace sm_dance_bot_3

using namespace sm_dance_bot_3::radial_motion_states;

// STATE DECLARATION
struct SsRadialPattern3 : smacc::SmaccState<SsRadialPattern3, MsDanceBotRunMode, StiRadialLoopStart>
{
public:
    using SmaccState::SmaccState;

// TRANSITION TABLE
    typedef mpl::list<
    
    Transition<EvLoopEnd<StiRadialLoopStart>, StRotateDegrees4, ENDLOOP> 

    >reactions;

// STATE FUNCTIONS
    static void staticConfigure()
    {
        //configure_orthogonal<OrObstaclePerception, CbLidarSensor>();
    }

    int iteration_count;

    static constexpr int total_iterations() { return 20; }
    static constexpr float ray_angle_increment_degree() { return 360.0 / total_iterations(); }
    static constexpr float ray_length_meters() { return 4; }

    void runtimeConfigure()
    {
        iteration_count = 0;
    }
};

//forward declaration for the superstate
using SS = SsRadialPattern3;
#include <sm_dance_bot_3/states/radial_motion_states/sti_radial_end_point.h>
#include <sm_dance_bot_3/states/radial_motion_states/sti_radial_return.h>
#include <sm_dance_bot_3/states/radial_motion_states/sti_radial_rotate.h>
#include <sm_dance_bot_3/states/radial_motion_states/sti_radial_loop_start.h>
} // namespace SS3
} // namespace sm_dance_bot_3