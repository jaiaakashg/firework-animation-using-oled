#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 
#define SCREEN_HEIGHT 64 

#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C 

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define NUM_PARTICLES 100
#define NUM_FIREWORKS 5

struct Particle {
  float x, y;
  float vx, vy;
  int life;
};

struct Firework {
  int x, y;
  int peakY; 
  Particle particles[NUM_PARTICLES];
  bool active;
  bool exploded;
  int rocketLife;
};

Firework fireworks[NUM_FIREWORKS];

void setup() {
  Serial.begin(115200);
  // Initialize the OLED display
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }
  display.clearDisplay();
  display.display();

  for (int i = 0; i < NUM_FIREWORKS; i++) {
    fireworks[i].active = false;
  }
}

void loop() {
  display.clearDisplay();

  for (int i = 0; i < NUM_FIREWORKS; i++) {
    if (!fireworks[i].active) {
      startFirework(i);
    }
    updateFirework(i);
  }

  display.display();
  delay(20); 
}

void startFirework(int index) {
  fireworks[index].x = random(20, SCREEN_WIDTH - 20);
  fireworks[index].y = SCREEN_HEIGHT;
  fireworks[index].peakY = random(SCREEN_HEIGHT / 2, SCREEN_HEIGHT * 3 / 4); 
  fireworks[index].rocketLife = random(20, 50);
  fireworks[index].exploded = false;
  fireworks[index].active = true;

  for (int i = 0; i < NUM_PARTICLES; i++) {
    float angle = random(0, 360) * PI / 180;
    float speed = random(5, 15) / 10.0; 
    fireworks[index].particles[i].x = fireworks[index].x;
    fireworks[index].particles[i].y = fireworks[index].y;
    fireworks[index].particles[i].vx = speed * cos(angle);
    fireworks[index].particles[i].vy = speed * sin(angle);
    fireworks[index].particles[i].life = random(20, 50);
  }
}

void updateFirework(int index) {
  Firework &fw = fireworks[index];

  if (!fw.exploded) {
    if (fw.rocketLife > 0) {
      fw.rocketLife--;
      fw.y = map(fw.rocketLife, 0, 50, fw.y, fw.peakY); 
      fw.y -= 3; 

      display.fillRect(fw.x - 1, fw.y - 3, 3, 6, SSD1306_WHITE); 
      display.fillTriangle(fw.x - 2, fw.y - 3, fw.x + 2, fw.y - 3, fw.x, fw.y - 6, SSD1306_WHITE);
    } else {
      fw.exploded = true;
    
      for (int i = 0; i < NUM_PARTICLES; i++) {
        fw.particles[i].x = fw.x;
        fw.particles[i].y = fw.peakY;
        float angle = random(0, 360) * PI / 180;
        float speed = random(5, 15) / 10.0; 
        fw.particles[i].vx = speed * cos(angle);
        fw.particles[i].vy = speed * sin(angle);
        fw.particles[i].life = random(20, 50);
      }
    }
  } else {
    bool allParticlesDead = true;
    for (int i = 0; i < NUM_PARTICLES; i++) {
      Particle &p = fw.particles[i];
      if (p.life > 0) {
        p.life--;
        p.x += p.vx;
        p.y += p.vy;
        p.vx *= 0.95; 
        p.vy *= 0.95;
        if (p.life > 0) {
          int brightness = map(p.life, 0, 50, 0, 255); 
          display.drawPixel(p.x, p.y, brightness > 127 ? SSD1306_WHITE : SSD1306_BLACK);
        }
        allParticlesDead = false;
      }
    }
    if (allParticlesDead) {
      fw.active = false;
    }
  }
}

