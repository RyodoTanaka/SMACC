#pragma once
#include <smacc/smacc_state.h>
#include <smacc/smacc_orthogonal.h>
#include <smacc/smacc_client_behavior.h>
#include <smacc/logic_units/logic_unit_base.h>
#include <smacc/introspection/string_type_walker.h>
#include <smacc/smacc_client_behavior.h>
#include <smacc/smacc_state_machine.h>

namespace smacc
{
//-------------------------------------------------------------------------------------------------------
// Delegates to ROS param access with the current NodeHandle
template <typename T>
bool ISmaccState::getParam(std::string param_name, T &param_storage)
{
    return nh.getParam(param_name, param_storage);
}
//-------------------------------------------------------------------------------------------------------

// Delegates to ROS param access with the current NodeHandle
template <typename T>
void ISmaccState::setParam(std::string param_name, T param_val)
{
    return nh.setParam(param_name, param_val);
}
//-------------------------------------------------------------------------------------------------------

//Delegates to ROS param access with the current NodeHandle
template <typename T>
bool ISmaccState::param(std::string param_name, T &param_val, const T &default_val) const
{
    return nh.param(param_name, param_val, default_val);
}
//-------------------------------------------------------------------------------------------------------

template <typename TOrthogonal, typename TBehavior, typename... Args>
void ISmaccState::configure(Args &&... args)
{
    std::string orthogonalkey = demangledTypeName<TOrthogonal>();
    ROS_INFO("Configuring orthogonal: %s", orthogonalkey.c_str());

    std::shared_ptr<TOrthogonal> orthogonal;
    if (this->getStateMachine().getOrthogonal<TOrthogonal>(orthogonal))
    {
        auto smaccBehavior = std::shared_ptr<TBehavior>(new TBehavior(args...));
        smaccBehavior->template assignToOrthogonal<TBehavior, TOrthogonal>();
        orthogonal->setStateBehavior(smaccBehavior);
    }
    else
    {
        ROS_ERROR("Skipping client behavior creation. Orthogonal did not exist.");
    }
}
//-------------------------------------------------------------------------------------------------------

template <typename SmaccComponentType>
void ISmaccState::requiresComponent(SmaccComponentType *&storage)
{
    this->getStateMachine().requiresComponent(storage);
}
//-------------------------------------------------------------------------------------------------------

template <typename SmaccClientType>
void ISmaccState::requiresClient(SmaccClientType *&storage, bool verbose)
{
    storage = nullptr;
    auto &orthogonals = this->getStateMachine().getOrthogonals();
    for (auto &ortho : orthogonals)
    {
        ortho.second->requiresClient(storage, verbose);
        if (storage != nullptr)
            return;
    }

    ROS_ERROR("Client of type '%s' not found in any orthogonal of the current state machine. This may produce a segmentation fault if the returned reference is used.", demangleSymbol<SmaccClientType>().c_str());
}
//-------------------------------------------------------------------------------------------------------

template <typename T>
bool ISmaccState::getGlobalSMData(std::string name, T &ret)
{
    return this->getStateMachine().getGlobalSMData(name, ret);
}
//-------------------------------------------------------------------------------------------------------

// Store globally in this state machine. (By value parameter )
template <typename T>
void ISmaccState::setGlobalSMData(std::string name, T value)
{
    this->getStateMachine().setGlobalSMData(name, value);
}
//-------------------------------------------------------------------------------------------------------

template <typename TLUnit, typename TTriggerEvent, typename TEventList, typename... TEvArgs>
std::shared_ptr<TLUnit> ISmaccState::createLogicUnit(TEvArgs... args)
{
    auto lu = std::make_shared<TLUnit>(args...);
    TEventList *mock;
    lu->initialize(this, mock);
    lu->declarePostEvent(typelist<TTriggerEvent>());
    logicUnits_.push_back(lu);
    return lu;
}

// template <typename TLUnit, typename TTriggerEvent, typename... TEvArgs>
// std::shared_ptr<TLUnit> ISmaccState::createLogicUnit(TEvArgs... args)
// {
//     auto lu = std::make_shared<TLUnit>(std::forward(args...));
//     lu->initialize(this, typelist<TEvArgs...>());
//     lu->declarePostEvent(typelist<TTriggerEvent>());
//     logicUnits_.push_back(lu);

//     return lu;
// }
//-------------------------------------------------------------------------------------------------------

template <typename EventType>
void ISmaccState::postEvent(const EventType &ev)
{
    getStateMachine().postEvent(ev);
}
//-------------------------------------------------------------------------------------------------------

template <typename TransitionType>
void ISmaccState::notifyTransition()
{
    auto transitionType = smacc::TypeInfo::getTypeInfoFromType<TransitionType>();
    this->notifyTransitionFromTransitionTypeInfo(transitionType);
}

//-------------------------------------------------------------------------------------------------------
template <typename TEv>
void LogicUnit::declarePostEvent(typelist<TEv>)
{
    this->postEventFn = [this]() {
        ROS_INFO_STREAM("[Logic Unit Base] postingfn posting event: " << demangleSymbol<TEv>());
        auto *ev = new TEv();
        this->ownerState->getStateMachine().postEvent(ev);
    };
}

//-------------------------------------------------------------------------------------------------------------------

} // namespace smacc