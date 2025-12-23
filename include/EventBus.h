#pragma once
#include "DamageContext.h"
#include "Enums.h"
#include <functional>
#include <map>
#include <vector>

using EventCallback = std::function<void(DamageContext &)>;

class EventBus {
public:
  static EventBus &getInstance();

  void subscribe(EventType type, EventCallback callback);
  void dispatch(EventType type, DamageContext &ctx);
  void reset();

private:
  std::map<EventType, std::vector<EventCallback>> listeners;
};
