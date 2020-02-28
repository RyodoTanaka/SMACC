#include <move_base_z_client_plugin/components/costmap_switch/cp_costmap_switch.h>

namespace cl_move_base_z
{

std::array<std::string, 4>
    CostmapSwitch::layerNames =
        {
            "global_costmap/obstacles_layer",
            "local_costmap/obstacles_layer"
            "global_costmap/inflater_layer",
            "local_costmap/inflater_layer"};

void CostmapSwitch::registerProxyFromDynamicReconfigureServer(std::string costmapName, std::string enablePropertyName)
{
    ROS_INFO("[CostmapSwitch] registering costmap type: %s", costmapName.c_str());
    auto proxy = std::make_shared<CostmapProxy>(this->owner_->name_ + "/" + costmapName, enablePropertyName);
    costmapProxies[costmapName] = proxy;
}

CostmapSwitch::CostmapSwitch()
{
}

void CostmapSwitch::initialize(smacc::ISmaccClient *owner)
{
    this->owner_ = dynamic_cast<cl_move_base_z::ClMoveBaseZ *>(owner);

    if (this->owner_ == nullptr)
    {
        ROS_ERROR("the owner of the CostmapSwitch must be a ClMoveBaseZ");
    }

    registerProxyFromDynamicReconfigureServer(getStandardCostmapName(StandardLayers::GLOBAL_OBSTACLES_LAYER));
    registerProxyFromDynamicReconfigureServer(getStandardCostmapName(StandardLayers::LOCAL_OBSTACLES_LAYER));
    registerProxyFromDynamicReconfigureServer(getStandardCostmapName(StandardLayers::GLOBAL_INFLATED_LAYER));
    registerProxyFromDynamicReconfigureServer(getStandardCostmapName(StandardLayers::LOCAL_INFLATED_LAYER));
}

std::string CostmapSwitch::getStandardCostmapName(StandardLayers layertype)
{
    return CostmapSwitch::layerNames[(int)layertype];
}

bool CostmapSwitch::exists(std::string layerName)
{
<<<<<<< HEAD
    if (!this->costmapProxies.count(layerName))
    {
=======
    if (!CostmapSwitch::costmapProxies.count(layerName))
>>>>>>> 8789b6c9c0789a1ed6faa6b8a7e60f65711388e5
        return false;
    }

    return true;
}

void CostmapSwitch::enable(std::string layerName)
{
    ROS_INFO("[CostmapSwitch] enabling %s", layerName.c_str());

    if (!exists(layerName))
    {
        ROS_ERROR("[CostmapSwitch] costmap %s does not exist", layerName.c_str());
        return;
    }
    else
    {
        ROS_INFO("[CostmapSwitch] costmap %s found. Calling dynamic reconfigure server.", layerName.c_str());
        costmapProxies[layerName]->setCostmapEnabled(true);
    }
}

void CostmapSwitch::enable(StandardLayers layerType)
{
    this->enable(getStandardCostmapName(layerType));
}

void CostmapSwitch::disable(std::string layerName)
{
    ROS_INFO("[CostmapSwitch] disabling %s", layerName.c_str());

    if (!exists(layerName))
    {
        ROS_ERROR("[CostmapSwitch] costmap %s does not exist", layerName.c_str());
        return;
    }
    else
    {
        ROS_INFO("[CostmapSwitch] costmap %s found. Calling dynamic reconfigure server.", layerName.c_str());
        costmapProxies[layerName]->setCostmapEnabled(false);
    }
}

void CostmapSwitch::disable(StandardLayers layerType)
{
    this->disable(getStandardCostmapName(layerType));
}

//-------------------------------------------------------------------------

CostmapProxy::CostmapProxy(std::string costmap_name, std::string enablePropertyName)
{
    this->costmapName_ = costmap_name + "/set_parameters";
    dynamic_reconfigure::BoolParameter enableField;
    enableField.name = "enabled";
    enableField.value = true;

    enableReq.bools.push_back(enableField);

    enableField.value = false;
    disableReq.bools.push_back(enableField);
}

void CostmapProxy::setCostmapEnabled(bool value)
{
    dynamic_reconfigure::ReconfigureRequest srv_req;
    dynamic_reconfigure::ReconfigureResponse srv_resp;

    if (value)
        srv_req.config = enableReq;
    else
        srv_req.config = disableReq;

    if (ros::service::exists(costmapName_, true))
    {
        ROS_INFO("sending dynamic reconfigure request: %s", costmapName_.c_str());
        ros::service::call(costmapName_, srv_req, srv_resp);
    }
    else
    {
        ROS_WARN("could not call dynamic reconfigure server. It does not exist: %s", costmapName_.c_str());
    }
}

void CostmapProxy::dynreconfCallback(const dynamic_reconfigure::Config::ConstPtr &configuration_update)
{
    // auto gp = std::find_if(configuration_update->strs.begin(), configuration_update->strs.begin(),
    //                        [&](const dynamic_reconfigure::StrParameter &p) { return p.name == "base_global_planner" && p.value == desired_global_planner_; });
}
}