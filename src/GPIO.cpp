#include <Arduino.h>
#include <SimpleGPIO/GPIO.h>

namespace SimpleGPIO
{
GPIO::GPIO(uint8_t pin) : pin_(pin) {}

uint8_t GPIO::pin()
{
   return pin_;
}

InputPin::InputPin(uint8_t pin) : GPIO(pin) {}

DigitalPin::DigitalPin(uint8_t pin) : GPIO(pin) {}

DigitalInputPin::DigitalInputPin(uint8_t digitalInputPin,
                                 DigitalInputMode inputMode,
                                 InvertMode invertMode)
    : GPIO(digitalInputPin)
    , InputPin(digitalInputPin)
    , DigitalPin(digitalInputPin)
    , invertMode_(invertMode)
{
   pinMode(pin(), digitalInputModeGenericToArduino(inputMode));

   lastState_ = readState();
}

DigitalPinState DigitalInputPin::state()
{
   poll(nullptr);
   return lastState_;
}

void DigitalInputPin::poll()
{
   poll(nullptr);
}

void DigitalInputPin::poll(
    std::function<void(PinEvent, DigitalPinState)> callback)
{
   auto currentState = readState();

   if (currentState != lastState_)
   {
      if (onChangeCallback_) onChangeCallback_(currentState);
      if (currentState == DigitalPinState::High)
      {
         if (callback) callback(PinEvent::Rise, currentState);
         if (onRiseCallback_) onRiseCallback_();
      }
      else
      {
         if (callback) callback(PinEvent::Fall, currentState);
         if (onFallCallback_) onFallCallback_();
      }
      lastState_ = currentState;
   }
}

void DigitalInputPin::onFall(std::function<void()> callback)
{
   onFallCallback_ = callback;
}

void DigitalInputPin::onRise(std::function<void()> callback)
{
   onRiseCallback_ = callback;
}

void DigitalInputPin::onChange(std::function<void(DigitalPinState)> callback)
{
   onChangeCallback_ = callback;
}

DigitalPinState DigitalInputPin::readState()
{
   bool currentValue = digitalRead(pin());
   if (invertMode_ == InvertMode::Invert)
   {
      currentValue = !currentValue;
   }

   return pinStateArduinoToGeneric(currentValue);
}

int digitalInputModeGenericToArduino(DigitalInputMode inputMode)
{
   switch (inputMode)
   {
      case DigitalInputMode::Input:
         return INPUT_PULLDOWN;
      case DigitalInputMode::Pulldown:
         return INPUT_PULLDOWN;
      case DigitalInputMode::Pullup:
         return INPUT_PULLUP;
   }

   return 0;
}

DigitalPinState pinStateArduinoToGeneric(bool value)
{
   if (value)
      return DigitalPinState::High;
   else
      return DigitalPinState::Low;
}

}  // namespace SimpleGPIO