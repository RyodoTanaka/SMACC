struct SsrSPatternForward2 : public smacc::SmaccState<SsrSPatternForward2, SS>
{
  using SmaccState::SmaccState;

  typedef mpl::list<smacc::transition<EvActionSucceeded<smacc::SmaccMoveBaseActionClient, NavigationOrthogonal>, SsrSPatternRotate3>,
                    smacc::transition<EvActionAborted<smacc::SmaccMoveBaseActionClient, NavigationOrthogonal>, SsrSPatternRotate2>
                    > reactions;

  static void onDefinition()
  {
  }

  void onInitialize()
  {
    auto &superstate = this->context<SS>();

    this->configure<NavigationOrthogonal, CbNavigateForward>(SS::pitch1_lenght_meters());
    this->configure<ToolOrthogonal, CbToolStart>();
  }
};