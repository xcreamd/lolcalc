#include "EventBus.h"

EventBus &EventBus::getInstance() {
  static EventBus instance;
  return instance;
}

void EventBus::subscribe(EventType type, EventCallback callback) {
  listeners[type].push_back(callback);
}

void EventBus::dispatch(EventType type, DamageContext &ctx) {
  if (listeners.count(type)) {
    for (auto &callback : listeners[type]) {
      callback(ctx);
    }
  }
}

void EventBus::reset() { listeners.clear(); }
