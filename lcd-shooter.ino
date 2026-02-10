#include <Wire.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27,16,2);

int bossPatern = -1;

int lastBsPtrn = 0;

unsigned long lastMove = 0;
int moveDelay = 150;



int health = 3;
int score = 0;
int playerRow = 0;
int playerCol = 0;

// joystick
int VRy = A1;
int SW = 32;


// nepřátelé
struct Enemy {
  int row;
  int col;
  bool alive;
  bool exploding;
  unsigned long explodeStart;
};
Enemy enemies[3] = {{0,15,true,false,0},{1,14,true,false,0},{1,13,true,false,0}};
//  boss strely
struct bossBullet {
  int row;
  int col;
  bool active;
};
bossBullet bulletsB[6];//max boss strel
// střely
struct Bullet {
  int row;
  int col;
  bool active;
};
Bullet bullets[10]; // max 10 střel

unsigned long lastShot = 0;
int shotDelay = 700;

// animace výbuchu
char explosionFrames[] = {'X','|','-','+','*'};
int explosionLength = 5;
int frameDelay = 120; // ms na frame

// boss
struct Boss {
  int row;
  int col;
  int health;
  bool alive;
  bool exploding;
  unsigned long explodeStart;
};
Boss boss = {0,4,5,false,false,0};
bool bossSpawnedOnce = false;

unsigned long lastBossShot = 0;
int bossShotDelay = 600;



int enemySpeed = max(0,10-score/5);
int enemyCounter = 0;
int bossShootDelay = 0;



void spawnBossBullet(int col, int row){
  for(int i = 0; i < 6; i++){     
    if(!bulletsB[i].active){      
      bulletsB[i].col = col;       
      bulletsB[i].row = row;      
      bulletsB[i].active = true;  
      break;                       
    }
  }
}
void showSucker(){
  lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("   !!SUCKER!!");
            lcd.setCursor(0,1);
            lcd.print("  !!!SUCKER!!!");
            delay(800);
}


void setup() {
  Serial.begin(9600);
  lcd.init();
  lcd.backlight();
  pinMode(SW, INPUT_PULLUP);
  randomSeed(analogRead(A0));
}

void loop() {
  
  if(score/20 > lastBsPtrn){
    bossPatern++;
    lastBsPtrn = score/20;
  }


  int yValue = analogRead(VRy);
  int buttonState = digitalRead(SW);
  if(millis() - lastMove >= moveDelay){
    if(yValue < 300 && playerRow !=0){
      playerRow = 0;
      lastMove = millis();
    }
    else if(yValue > 700 && playerRow != 1){
      playerRow = 1;
      lastMove = millis();
    }
  }

  
  // --- GAME OVER ---
  if(health <= 0){
    showSucker();
    lcd.clear();
    lcd.setCursor(2,0);
    lcd.print("GAME OVER");
    lcd.setCursor(1,1);
    lcd.print("Score: ");
    lcd.print(score);
    while(true);
  }

  // --- střílení hráče ---
  if(buttonState == LOW && millis() - lastShot >= shotDelay){
    for(int i=0;i<10;i++){
      if(!bullets[i].active){
        bullets[i].row = playerRow;
        bullets[i].col = playerCol + 1;
        bullets[i].active = true;
        lastShot = millis();
        break;
      }
    }
  }

  // --- posun střel hráče ---
  for(int i=0;i<10;i++){
    if(bullets[i].active){
      bullets[i].col++;
      if(bullets[i].col > 15) bullets[i].active = false;
    }
  }

  //strely bosse 
  
  
  if(boss.alive && millis() - lastBossShot >= bossShotDelay){
    switch(bossPatern){
    case 0:
    spawnBossBullet(boss.col-1, playerRow);
    break;
    case 1 :
    int randBbulletrow = random(0,2);
    spawnBossBullet(boss.col-1,randBbulletrow);
    spawnBossBullet(boss.col-2,randBbulletrow); 
    break;
    default:
    spawnBossBullet(boss.col - 1, random(0,2));
    break;   
    }
    lastBossShot = millis();
  }
  
    
  
  //pohyb strely bosse
  for(int i = 0; i<6;i++){
    if(bulletsB[i].active){
      bulletsB[i].col--;
      if(bulletsB[i].col < 0) bulletsB[i].active = false;
      
    }
  }

  // damage od bosse
  for(int i = 0; i< 6 ; i++){
    if(bulletsB[i].active && bulletsB[i].row == playerRow && bulletsB[i].col <= 0){
      health-=1;
      break;
    }
  }

  // --- spawn bosse ---
  if(score % 20 == 0 && score > 0 && !boss.alive){
    boss.alive = true;
    boss.row = 0;
    boss.col = 13;
    boss.health = 5;
    bossSpawnedOnce = true;
    // zastavit normální nepřátele během bosse
    
  }

  // --- posun nepřátel ---
  if(!boss.alive){
    enemyCounter++;
    if(enemyCounter >= enemySpeed){
      for(int i=0;i<3;i++){
        if(enemies[i].alive && !enemies[i].exploding){
          enemies[i].col--;
          if(enemies[i].col < 0){
            enemies[i].alive = false;
            health--;
            score--;
            showSucker();
            enemies[i].alive = true;
            enemies[i].col = 15;
            enemies[i].row = random(0,2);
          }
        }
      }
      enemyCounter = 0;
    }
  }

  // --- detekce kolize hráč-střela ---
  for(int i=0;i<3;i++){
    for(int j=0;j<10;j++){
      if(enemies[i].alive && !enemies[i].exploding &&
         bullets[j].active &&
         bullets[j].row == enemies[i].row &&
         bullets[j].col >= enemies[i].col)
      {
        enemies[i].exploding = true;
        enemies[i].explodeStart = millis();
        bullets[j].active = false;
        score++;
      }
    }
  }

  // --- boss kolize ---
  if(boss.alive && !boss.exploding){
    for(int i=0;i<10;i++){
      if(bullets[i].active &&
         bullets[i].row == boss.row &&
         bullets[i].col >= boss.col)
      {
        boss.health--;
        bullets[i].active = false;
        if(boss.health <= 0){
          boss.exploding = true;
          boss.explodeStart = millis();
          boss.alive = false;
          score += 5;
        }
      }
    }
  }

  // --- respawn nepřátel ---
  for(int i=0;i<3;i++){
    if(!enemies[i].alive && !boss.alive){
      if(random(0,100)<5){
        enemies[i].alive = true;
        enemies[i].col = 15;
        enemies[i].row = random(0,2);
      }
    }
  }

  // --- vykreslení ---
  lcd.clear();

  // hráč
  lcd.setCursor(playerCol,playerRow);
  if(health == 3){
    lcd.print("|->");   
    }
    else if(health == 2){
      lcd.print("|>");
  }
  else if(health == 1){
    lcd.print(">");

  }
  

  // hráč střely
  for(int i=0;i<10;i++){
    if(bullets[i].active){
      lcd.setCursor(bullets[i].col, bullets[i].row);
      lcd.print("-");
    }
  }
  //boss strely
  for(int i=0;i<6;i++){
    if(bulletsB[i].active){
      lcd.setCursor(bulletsB[i].col,bulletsB[i].row);
      lcd.print("*");
    }
  }

  // nepřátelé + exploze
  for(int i=0;i<3;i++){
    if(enemies[i].alive && !enemies[i].exploding && !boss.alive){
      lcd.setCursor(enemies[i].col, enemies[i].row);
      lcd.print("X");
    } else if(enemies[i].exploding){
      unsigned long elapsed = millis() - enemies[i].explodeStart;
      int currentFrame = elapsed / frameDelay;
      if(currentFrame < explosionLength){
        lcd.setCursor(enemies[i].col, enemies[i].row);
        lcd.print(explosionFrames[currentFrame]);
      } else {
        enemies[i].exploding = false;
        enemies[i].alive = false;
      }
    }
  }

  // boss +  životy
  if(boss.alive){
    // aura
    lcd.setCursor(boss.col-2,0);
    lcd.print("O  O");
    lcd.setCursor(boss.col-2,1);
    lcd.print("<==>");
    
    // životy
    lcd.setCursor(4,0);
    lcd.print(boss.health);
  }
  if(boss.exploding){
  unsigned long elapsed = millis() - boss.explodeStart;
  int currentFrame = elapsed / frameDelay;
  if(currentFrame < explosionLength){
    lcd.setCursor(boss.col, boss.row);
     lcd.print(explosionFrames[currentFrame]);
   } else {
     boss.exploding = false;
     boss.alive = false;
     // po smrti bosse normální nepřátelé zpět
     for(int i=0;i<3;i++) enemies[i].alive = true;
   }
  }

  // skóre
  if(score < 10) lcd.setCursor(15,0);
  else if(score < 100) lcd.setCursor(14,0);
  else lcd.setCursor(13,0);
  lcd.print(score);
  

  delay(150);

  // dynamická rychlost nepřátel podle skóre
  if(!boss.alive) enemySpeed = max(2, 10 - score/5);
}
