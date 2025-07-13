#include <TMCStepper.h>

// Pin-Definitionen
#define CS_PIN    15
#define MOSI_PIN  12
#define MISO_PIN  21
#define SCK_PIN   13
#define EN_PIN    54

// Referenz­widerstand an IREF (Ohm)
#define R_REF 12000.0f

// Parameter
const uint16_t runCurrentmA   = 2000;   // Motor-RMS-Strom in mA
const float    holdMultiplier = 0.8;    // Haltestrom = 80 % des Laufstroms
const int8_t   stallValue     = -63;      // StallGuard-Empfindlichkeit [-64..63]
const uint8_t  icsRange       = 1;      // ICS-Strombereich: 1→2 A RMS

constexpr uint16_t microsteps   = 256;  // Microstep-Auflösung
constexpr uint16_t stepsPerRev  = 200;  // Vollschritte/Umdrehung
const int32_t    halfRotation  = (int32_t)stepsPerRev * microsteps / 2;
const int32_t    fullRotation  = halfRotation * 2;

TMC5240Stepper driver(CS_PIN, R_REF, MOSI_PIN, MISO_PIN, SCK_PIN);

// Statusanzeige
void printDriverStatus() {
  TMC2130_n::DRV_STATUS_t s{0};
  s.sr = driver.DRV_STATUS();
  Serial.print("SG_RESULT="); Serial.print(s.sg_result);
  Serial.print(" CS_ACTUAL="); Serial.print(s.cs_actual);
  Serial.print(" (~"); Serial.print(driver.cs2rms(s.cs_actual)); Serial.print(" mA)");
  Serial.print(" STALL="); Serial.print(s.stallGuard);
  Serial.print(" OT=");    Serial.print(s.ot);
  Serial.print(" OTPW=");  Serial.print(s.otpw);
  Serial.println();
}

void setup() {
  pinMode(EN_PIN, OUTPUT);
  digitalWrite(EN_PIN, LOW);       // Treiber einschalten

  SPI.begin();
  Serial.begin(115200);
  while(!Serial);

  Serial.println("=== TMC5240: Endlos Stall–Rück–Halbe–Loop ===");

  // Grundkonfiguration
  driver.begin();
  driver.filt_isense(icsRange);
  driver.toff(5);
  driver.blank_time(24);
  driver.microsteps(microsteps);
  driver.rms_current(runCurrentmA, holdMultiplier);
  driver.en_pwm_mode(true);
  driver.pwm_autoscale(true);

  // StallGuard
  driver.TCOOLTHRS(0xFFFFF);
  driver.semin(5);
  driver.semax(2);
  driver.sedn(0b01);
  driver.sgt(stallValue);

  // Rampengenerator
  driver.RAMPMODE(0);
  driver.AMAX(4000);
  driver.DMAX(4000);
  driver.VMAX(40000);
  driver.VSTART(0);
  driver.VSTOP(10);

  driver.XACTUAL(0);
}

void loop() {
  // Sicherstellen, dass Stall-Flag zurückgesetzt ist
  while (driver.status_sg()) {
    // warten, bis StallGuard frei ist
  }

  // Vorwärtsfahrt bis Stall
  Serial.println("\n→ Fahre vorwärts bis Stall …");
  int32_t currentPos = driver.XACTUAL();
  driver.XTARGET(currentPos + fullRotation * 2); // genügend Weg
  while (!driver.status_sg()) {
    // warten, bis StallGuard anspricht
  }
  Serial.println("⚠️ Stall erkannt!");
  printDriverStatus();

  // Sofort halbe Umdrehung rückwärts
  Serial.println("← Drehe halbe Umdrehung rückwärts …");
  int32_t backTarget = driver.XACTUAL() - halfRotation;
  driver.XTARGET(backTarget);
  while (!driver.position_reached()) {
    // warten, bis Ziel erreicht
  }
  Serial.println("✅ Halbe Umdrehung zurück erreicht.");
  printDriverStatus();

  // Loop beginnt automatisch von vorn
}
