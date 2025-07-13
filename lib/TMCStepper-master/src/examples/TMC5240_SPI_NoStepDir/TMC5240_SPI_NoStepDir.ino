#include <TMCStepper.h>

// Pin definitions
#define CS_PIN    15
#define MOSI_PIN  12
#define MISO_PIN  21
#define SCK_PIN   13
#define EN_PIN    54

// Current sense resistor used on the driver board
#define R_SENSE 12000.0f

// Configurable parameters
const uint16_t runCurrentmA = 800;        // motor RMS current in mA
const float holdMultiplier   = 0.5;       // hold current as a fraction of run current
const int8_t stallValue      = 4;         // StallGuard sensitivity [-64..63]

constexpr uint16_t microsteps = 256;      // microstep resolution
constexpr uint16_t stepsPerRev = 200;     // motor full steps per revolution
const int32_t halfRotation = (int32_t)stepsPerRev * microsteps / 2;

TMC5240Stepper driver(CS_PIN, R_SENSE, MOSI_PIN, MISO_PIN, SCK_PIN);

// Helper that reads DRV_STATUS and prints useful information
void printDriverStatus() {
  TMC2130_n::DRV_STATUS_t status{0};
  status.sr = driver.DRV_STATUS();

  Serial.print("SG_RESULT=");
  Serial.print(status.sg_result);
  Serial.print(" CS_ACTUAL=");
  Serial.print(status.cs_actual);
  Serial.print(" (~");
  Serial.print(driver.cs2rms(status.cs_actual));
  Serial.print("mA) CUR_A=");
  Serial.print(driver.cur_a());
  Serial.print(" CUR_B=");
  Serial.print(driver.cur_b());
  Serial.print(" STALL=");
  Serial.print(status.stallGuard);
  Serial.print(" OT=");
  Serial.print(status.ot);
  Serial.print(" OTPW=");
  Serial.print(status.otpw);
  Serial.println();
}

void setup() {
  pinMode(EN_PIN, OUTPUT);
  digitalWrite(EN_PIN, LOW);  // Enable driver

  SPI.begin();
  Serial.begin(115200);
  while(!Serial);
  Serial.println("TMC5240 example starting");

  Serial.print("Run current set to ");
  Serial.print(runCurrentmA);
  Serial.println(" mA RMS");
  Serial.print("Hold multiplier set to ");
  Serial.println(holdMultiplier);

  driver.begin();
  driver.toff(5);
  driver.blank_time(24);
  driver.microsteps(microsteps);         // Set microstep resolution
  driver.rms_current(runCurrentmA, holdMultiplier); // Set running and holding current
  driver.en_pwm_mode(true);              // Use stealthChop for quiet operation
  driver.pwm_autoscale(true);            // Automatically scale PWM values

  // StallGuard configuration
  driver.TCOOLTHRS(0xFFFFF);             // Enable StallGuard across all speeds
  driver.semin(5);                       // Lower threshold
  driver.semax(2);                       // Upper threshold
  driver.sedn(0b01);                     // Filter configuration
  driver.sgt(stallValue);                // Sensitivity value

  // Basic motion parameters for the internal ramp generator
  driver.RAMPMODE(0);             // Use position mode
  driver.AMAX(4000);              // Acceleration
  driver.DMAX(4000);              // Deceleration
  driver.VMAX(40000);             // Maximum velocity
  driver.VSTART(0);               // Start velocity
  driver.VSTOP(10);               // Stop velocity

  driver.XACTUAL(0);              // Start at position 0
}

void moveTo(int32_t target) {
  driver.XTARGET(target);
  while(!driver.position_reached()) {
    if(driver.status_sg()) {
        printDriverStatus();
      // Keep motor energized with defined current when stall detected
      driver.rms_current(runCurrentmA, holdMultiplier);

    }
  }
}

void loop() {
  moveTo(halfRotation);   // half turn clockwise
  printDriverStatus();
  delay(1000);
  moveTo(-halfRotation);  // half turn counter clockwise
  printDriverStatus();
  delay(1000);
}
