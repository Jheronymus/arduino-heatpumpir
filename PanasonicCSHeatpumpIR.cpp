#include <PanasonicCSHeatpumpIR.h>

// This is a protected method, i.e. generic Panasonic instances cannot be created
PanasonicCSHeatpumpIR::PanasonicCSHeatpumpIR() : HeatpumpIR()
{
  static const char PROGMEM model[] PROGMEM = "panasonic_cs";
  static const char PROGMEM info[]  PROGMEM = "{\"mdl\":\"panasonic_cs\",\"dn\":\"Panasonic CS\",\"mT\":16,\"xT\":30,\"fs\":6}";

  _model = model;
  _info = info;

  //_panasonicModel = PANASONIC_CS;
}


// The different models just set the model accordingly
//PanasonicCSHeatpumpIR::PanasonicCSHeatpumpIR() : PanasonicCSHeatpumpIR()
//{

//}


// Panasonic DKE/NKE/JKE numeric values to command bytes
void PanasonicCSHeatpumpIR::send(IRSender& IR, uint8_t powerModeCmd, uint8_t operatingModeCmd, uint8_t fanSpeedCmd, uint8_t temperatureCmd, uint8_t swingVCmd, uint8_t swingHCmd)
{
  send(IR, powerModeCmd, operatingModeCmd, fanSpeedCmd, temperatureCmd, swingVCmd, swingHCmd, false, false);
}

// Panasonic DKE/NKE/JKE numeric values to command bytes
void PanasonicCSHeatpumpIR::send(IRSender& IR, uint8_t powerModeCmd, uint8_t operatingModeCmd, uint8_t fanSpeedCmd, uint8_t temperatureCmd, uint8_t swingVCmd, uint8_t swingHCmd, bool powerfulCmd, bool quietCmd)
{
  // Sensible defaults for the heat pump mode

  uint8_t operatingMode = PANASONIC_AIRCON2_TIMER_CNL;
  uint8_t fanSpeed      = PANASONIC_AIRCON2_FAN1;
  uint8_t temperature   = 18;
  uint8_t swingV        = PANASONIC_AIRCON2_VS_UP;
  uint8_t swingH        = PANASONIC_AIRCON2_HS_RIGHT;

  switch (powerModeCmd)
  {
    case POWER_ON:
      operatingMode |= PANASONIC_AIRCON2_MODE_ON;
      break;
    case POWER_OFF:
      operatingMode |= PANASONIC_AIRCON2_MODE_OFF;
      break;
  }

  switch (operatingModeCmd)
  {
    case MODE_AUTO:
      operatingMode |= PANASONIC_AIRCON2_MODE_AUTO;
      break;
    case MODE_HEAT:
      operatingMode |= PANASONIC_AIRCON2_MODE_HEAT;
      break;
    case MODE_COOL:
      operatingMode |= PANASONIC_AIRCON2_MODE_COOL;
      break;
    case MODE_DRY:
      operatingMode |= PANASONIC_AIRCON2_MODE_DRY;
      break;
  }

  switch (fanSpeedCmd)
  {
    case FAN_AUTO:
      fanSpeed = PANASONIC_AIRCON2_FAN_AUTO;
      break;
    case FAN_1:
      fanSpeed = PANASONIC_AIRCON2_FAN1;
      break;
    case FAN_2:
      fanSpeed = PANASONIC_AIRCON2_FAN2;
      break;
    case FAN_3:
      fanSpeed = PANASONIC_AIRCON2_FAN3;
      break;
    case FAN_4:
      fanSpeed = PANASONIC_AIRCON2_FAN4;
      break;
    case FAN_5:
      fanSpeed = PANASONIC_AIRCON2_FAN5;
      break;
  }

  if ( temperatureCmd > 15 && temperatureCmd < 31)
  {
    temperature = temperatureCmd;
  }

  switch (swingVCmd)
  {
    case VDIR_AUTO:
    case VDIR_SWING:
      swingV = PANASONIC_AIRCON2_VS_AUTO;
      break;
    case VDIR_UP:
      swingV = PANASONIC_AIRCON2_VS_UP;
      break;
    case VDIR_MUP:
      swingV = PANASONIC_AIRCON2_VS_MUP;
      break;
    case VDIR_MIDDLE:
      swingV = PANASONIC_AIRCON2_VS_MIDDLE;
      break;
    case VDIR_MDOWN:
      swingV = PANASONIC_AIRCON2_VS_MDOWN;
      break;
    case VDIR_DOWN:
      swingV = PANASONIC_AIRCON2_VS_DOWN;
      break;
  }

  switch (swingHCmd)
  {
    case HDIR_AUTO:
    case HDIR_SWING:
      swingH = PANASONIC_AIRCON2_HS_AUTO;
      break;
    case HDIR_MIDDLE:
      swingH = PANASONIC_AIRCON2_HS_MIDDLE;
      break;
    case HDIR_LEFT:
      swingH = PANASONIC_AIRCON2_HS_LEFT;
      break;
    case HDIR_MLEFT:
      swingH = PANASONIC_AIRCON2_HS_MLEFT;
      break;
    case HDIR_RIGHT:
      swingH = PANASONIC_AIRCON2_HS_RIGHT;
      break;
    case HDIR_MRIGHT:
      swingH = PANASONIC_AIRCON2_HS_MRIGHT;
      break;
  }

  sendPanasonic(IR, operatingMode, fanSpeed, temperature, swingV, swingH);
}

// Send the Panasonic CS code
void PanasonicCSHeatpumpIR::sendPanasonic(IRSender& IR, uint8_t operatingMode, uint8_t fanSpeed, uint8_t temperature, uint8_t swingV, uint8_t swingH)
{
  // Only bytes 13, 14, 16, 17 and 26 are modified
  static const uint8_t panasonicProgmemTemplate[27] PROGMEM = {
    0x02, 0x20, 0xE0, 0x04, 0x00, 0x00, 0x00, 0x06, 0x02, 0x20, 0xE0, 0x04, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x0E, 0xE0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
  //   0     1     2     3     4     5     6     7     8     9    10    11    12    13    14   15     16    17    18    19    20    21    22    23    24    25    26
  };

  // Save some SRAM by having the template in flash
  uint8_t panasonicTemplate[27];
  memcpy_P(panasonicTemplate, panasonicProgmemTemplate, sizeof(panasonicTemplate));

  //switch(_panasonicModel)
  //{
  //  case PANASONIC_CS:
  //      // no model based changes
  //      break;
  //}
  // this gave out of scope build errors?

  panasonicTemplate[13] |= operatingMode;
  panasonicTemplate[14] = temperature << 1;
  panasonicTemplate[16] = fanSpeed | swingV;
  panasonicTemplate[17] = swingH;

  // Checksum calculation
  uint8_t checksum = 0x06;

  for (int i=13; i<26; i++) {
    checksum += panasonicTemplate[i];
  }

  panasonicTemplate[26] = checksum;

  // 38 kHz PWM frequency
  IR.setFrequency(38);

  // Header
  IR.mark(PANASONIC_AIRCON2_HDR_MARK);
  IR.space(PANASONIC_AIRCON2_HDR_SPACE);

  // First 8 bytes
  for (int i=0; i<8; i++) {
    IR.sendIRbyte(panasonicTemplate[i], PANASONIC_AIRCON2_BIT_MARK, PANASONIC_AIRCON2_ZERO_SPACE, PANASONIC_AIRCON2_ONE_SPACE);
  }

  // Pause
  IR.mark(PANASONIC_AIRCON2_BIT_MARK);
  IR.space(PANASONIC_AIRCON2_MSG_SPACE);

  // Header
  IR.mark(PANASONIC_AIRCON2_HDR_MARK);
  IR.space(PANASONIC_AIRCON2_HDR_SPACE);

  // Last 19 bytes
  for (int i=8; i<27; i++) {
    IR.sendIRbyte(panasonicTemplate[i], PANASONIC_AIRCON2_BIT_MARK, PANASONIC_AIRCON2_ZERO_SPACE, PANASONIC_AIRCON2_ONE_SPACE);
  }

  IR.mark(PANASONIC_AIRCON2_BIT_MARK);
  IR.space(0);
}