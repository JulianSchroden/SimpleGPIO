#ifndef _SIMPLE_GPIO_h
#define _SIMPLE_GPIO_h

#include <cstdint>
#include <functional>

enum class PinEvent
{
   Fall,
   Rise,
};

inline const char* toString(PinEvent event)
{
   switch (event)
   {
      case PinEvent::Fall:
         return "Fall";
      case PinEvent::Rise:
         return "Rise";
   }
   return "";
}

enum class DigitalInputMode
{
   Input,
   Pulldown,
   Pullup,
};

inline const char* toString(DigitalInputMode mode)
{
   switch (mode)
   {
      case DigitalInputMode::Input:
         return "Input";
      case DigitalInputMode::Pulldown:
         return "Pulldown";
      case DigitalInputMode::Pullup:
         return "Pullup";
   }
   return "";
}

enum class DigitalPinState
{
   Low,
   High,
};

inline const char* toString(DigitalPinState state)
{
   switch (state)
   {
      case DigitalPinState::Low:
         return "Low";
      case DigitalPinState::High:
         return "High";
   }
   return "";
}

enum class InvertMode
{
   NoInvert,
   Invert,
};

inline const char* toString(InvertMode mode)
{
   switch (mode)
   {
      case InvertMode::NoInvert:
         return "NoInvert";
      case InvertMode::Invert:
         return "Invert";
   }
   return "";
}

class GPIO
{
public:
   GPIO(uint8_t pin);
   virtual ~GPIO() = default;

   uint8_t pin();

private:
   uint8_t pin_;
};

class InputPin : virtual public GPIO
{
public:
   InputPin(uint8_t pin);

   virtual void poll() = 0;
};

class DigitalPin : virtual public GPIO
{
public:
   DigitalPin(uint8_t pin);

   virtual DigitalPinState state() = 0;
};

class DigitalInputPin
    : public InputPin
    , public DigitalPin
{
public:
   DigitalInputPin(uint8_t pin,
                   DigitalInputMode inputMode = DigitalInputMode::Input,
                   InvertMode invertMode = InvertMode::NoInvert);

   DigitalPinState state() override;

   void poll() override;
   void poll(std::function<void(PinEvent, DigitalPinState)> callback);

   void onFall(std::function<void()> callback);
   void onRise(std::function<void()> callback);
   void onChange(std::function<void(DigitalPinState)> callback);

private:
   DigitalPinState readState();

private:
   DigitalPinState lastState_ = DigitalPinState::Low;
   InvertMode invertMode_;
   std::function<void()> onFallCallback_;
   std::function<void()> onRiseCallback_;
   std::function<void(DigitalPinState)> onChangeCallback_;
};

int digitalInputModeGenericToArduino(DigitalInputMode inputMode);
DigitalPinState pinStateArduinoToGeneric(bool value);

#endif