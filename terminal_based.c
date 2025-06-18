#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>

#define ROW 5
#define COL 35

#define EASY 1
#define HARD 2

int row = ROW, col = COL;

struct Robot
{
    int x;
    int y;
    int health;
    char display;
    bool active;
};
struct ShieldedRobot
{
    int x;
    int y;
    int health;
    char display;
    bool active;
};

// Bullets
struct Bullet
{
    int x;
    int y;
    char display;
    bool active;
};
// Thorns
struct Thorn
{
    int x;
    int y;
    char display;
    bool active;
};

// Plants structures (Pea Shooter)
struct PeaShooter
{
    int x;
    int y;
    int health;
    char display;
    struct Bullet bullet;
    bool active;
};
// cactus
struct Cactus
{
    int x;
    int y;
    int health;
    char display;
    struct Thorn thorn;
    bool active;
};

// Global Variables
int n_robots = 4;
int n_sbots = 2;
int n_peashooter = 0;
int n_cactus = 0;

int robotTime = 0, bulletTime = 0, sbotTime = 0, thornTime = 0;
int thornDelay = 2, bulletDelay = 2, robotDelay = 10, sbotDelay = 15;

bool event1 = false, event2 = false, event3 = false, event4 = false, temp_event = false;
bool play = false; // Game running flag

// Game objects and grid
#define MAX_SIZE 100
char grid[ROW][COL];
struct PeaShooter peas[MAX_SIZE];
struct Cactus cacti[MAX_SIZE];
struct Robot robots[MAX_SIZE];
struct ShieldedRobot sbots[MAX_SIZE];

// Function Declarations
void initialize();
void input();
void update();
void draw();
void gameLoop();
void displayInstructions();
void mainMenu();
void clearScreen();

int main(void)
{
    mainMenu();
    return 0;
}

// Main Menu
void mainMenu()
{
    int choice;
    while (true)
    {

        printf("Roots vs Robots!\n\n ITS A WAR!\n\n");
        printf("1. Start Game\n\n");
        printf("2. How To Play\n\n");
        printf("3. Quit\n\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice)
        {
        case 1:
        {
            int mode;
            clearScreen();
            printf("Select Mode\n1. Easy\n2. Hard\n");
            scanf("%d", &mode);

            if (mode == EASY)
            {
                robotDelay = 20;
                sbotDelay = 15;
                gameLoop();
            }
            else if (mode == HARD)
            {
                robotDelay = 15;
                sbotDelay = 10;
                gameLoop();
            }
            else
            {
                printf("Invalid mode selected!\n");
                getchar();
            }
            break;
        }
        case 2:
            clearScreen();
            displayInstructions();
            break;
        case 3:
            printf("Exiting game. Goodbye!\n");
        default:
            printf("Invalid choice. Please select again.\n");
        }
    }
}

void clearScreen()
{
    system("clear"); // Clear the console
}

// Instructions
void displayInstructions()
{
    printf("\n--- How To Play ---\n");
    printf("1. Place plants ('P' for PeaShooter, 'C' for Cactus) by entering their type and coordinates.\n");
    printf("2. Robots ('R') and Shielded Robots ('S') will move from right to left.\n");
    printf("3. Use PeaShooters to shoot bullets ('o') and Cacti to shoot thorns ('-').\n");
    printf("4. Prevent robots from reaching the left edge of the grid.\n");
    printf("5. Enter 'r' to start the game, or 'q' to quit during setup.\n");
    printf("\nPress Enter to return to the main menu.\n");
    getchar(); // Pause for user input
}

// Game Loop
void gameLoop()
{
    initialize();

    char command;
    while (true)
    {
        if (play)
        {
            update();
            draw();
            usleep(100000); // Add delay between updates
        }
        else
        {
            // clearScreen();
            update();
            draw();
            printf("Enter 'r' to run the game, 'q' to quit, or place plants ('v' to place):\n");
            scanf(" %c", &command);

            if (command == 'r')
            {
                play = true; // Start the game
            }
            else if (command == 'q')
            {
                printf("Returning to main menu...\n");
                break; // Exit to main menu
            }
            else if (command == 'v')
            {
                input();
            }
            else
            {
                printf("invalid input\n");
                getchar(); // wait to display error
            }
        }
    }
}

// Initialize Game Objects
void initialize()
{
    for (int i = 0; i < n_robots; i++)
    {
        robots[i].x = col - 1;
        robots[i].health = 20;
        robots[i].display = 'R';
        robots[i].active = true;
        robots[i].y = i % row; // Distribute robots across rows
    }

    for (int i = 0; i < n_sbots; i++)
    {
        sbots[i].x = col - 1;
        sbots[i].health = 7;
        sbots[i].display = 'S';
        sbots[i].active = true;
        sbots[i].y = (i + 1) % row; // Distribute sbots across rows
    }
}

// Input for Plant Placement
void input()
{
    if (n_peashooter >= row || n_cactus >= row)
    {
        printf("Maximum number of plants reached!\n");
        return;
    }

    char type;
    int x, y;

    printf("Enter plant type ('P' or 'C') and coordinates (x y): ");
    scanf(" %c %d %d", &type, &x, &y);

    if (type == 'P' && x >= 0 && x < row && y >= 0 && y < col)
    {
        peas[n_peashooter].x = x;
        peas[n_peashooter].y = y;
        peas[n_peashooter].health = 5;
        peas[n_peashooter].display = 'p';
        peas[n_peashooter].active = true;
        peas[n_peashooter].bullet.x = x + 1;
        peas[n_peashooter].bullet.y = y;
        peas[n_peashooter].bullet.display = 'o';
        n_peashooter++;
    }
    else if (type == 'C' && x >= 0 && x < row && y >= 0 && y < col)
    {
        cacti[n_cactus].x = x;
        cacti[n_cactus].y = y;
        cacti[n_cactus].health = 5;
        cacti[n_cactus].display = 'c';
        cacti[n_cactus].active = true;
        cacti[n_cactus].thorn.x = x + 1;
        cacti[n_cactus].thorn.y = y;
        cacti[n_cactus].thorn.display = '-';
        n_cactus++;
    }
    else
    {
        printf("Invalid input.\n");
        getchar();
    }
}

// Game Logic Updates (Kept the same from your code)
void update()
{
    event1 = false;
    event2 = false;
    event3 = false;
    event4 = false;

    // Increment robot and bullet timers
    robotTime++;
    bulletTime++;
    sbotTime++;
    thornTime++;

    // Check if it's time to move the robot
    if (robotTime >= robotDelay)
    {
        robotTime = 0;
        event1 = true;
    }
    // Check if it's time to move the sbot
    if (sbotTime >= sbotDelay)
    {
        sbotTime = 0;
        event3 = true;
    }

    // Check if it's time to move the thorn
    if (thornTime >= thornDelay)
    {
        thornTime = 0;
        event4 = true;
    }

    // Check if it's time to move the bullet
    if (bulletTime >= bulletDelay)
    {
        bulletTime = 0;
        event2 = true;
    }

    // Robot movement
    for (int i = 0; i < n_robots; i++)
    {
        if (i % 2 != 0)
        {
            temp_event = event3;
        }
        else
        {
            temp_event = event1;
        }
        if (robots[i].active && temp_event)
        {

            robots[i].x -= 1;
            if (robots[i].x < 0)
            {
                robots[i].active = false; // end robot when reaches end
            }
            if (robots[i].x < 0)
            {
                robots[i].active = false; // end robot when reaches end
                printf("YOU LOSE! A robot reached the end.\n");
                play = false; // Stop the game
                return;       // Exit the game loop
            }
        }
    }

    // Bullet movement
    for (int j = 0; j < n_peashooter; j++)
    {
        if (peas[j].bullet.active && event2)
        {
            peas[j].bullet.x += 1;
            if (peas[j].bullet.x >= col)
            {
                peas[j].bullet.x = peas[j].x + 1; // Reset bullet to pea shooter position
            }
        }
    }

    // sbot movement
    for (int i = 0; i < n_sbots; i++)
    {
        if (i % 2 == 0)
        {
            temp_event = event1;
        }
        else
        {
            temp_event = event3;
        }

        if (sbots[i].active && temp_event)
        {
            sbots[i].x -= 1;
            if (sbots[i].x < 0)
            {
                sbots[i].active = false; // Loop sbot back to the right side
            }
            if (sbots[i].x < 0)
            {
                sbots[i].x = COL - 1; // Loop sbot back to the right side
                printf("YOU LOSE! A shielded robot reached the end.\n");
                play = false; // Stop the game
                return;       // Exit the game loop
            }
        }
    }

    // thorn movement
    for (int i = 0; i < n_cactus; i++)
    {
        if (cacti[i].thorn.active && event4)
        {
            cacti[i].thorn.x += 1;
            if (cacti[i].thorn.x >= col)
            {
                cacti[i].thorn.x = cacti[i].x + 1; // Reset thorn to cactus position
            }
        }
    }

    // collision detection between bullet and robot
    for (int i = 0; i < n_robots; i++)
    {
        for (int j = 0; j < n_peashooter; j++)
        {
            if (robots[i].active && peas[j].bullet.active && robots[i].x <= peas[j].bullet.x && robots[i].y == peas[j].bullet.y)
            {
                robots[i].health -= 1;            // Reduce robot health
                peas[j].bullet.x = peas[j].x + 1; // Reset bullet to initial position
            }
        }
    }

    // collision detection between thorn and sbot
    for (int i = 0; i < n_sbots; i++)
    {
        for (int j = 0; j < n_cactus; j++)
        {
            if (sbots[i].active && cacti[j].thorn.active && sbots[i].x <= cacti[j].thorn.x && sbots[i].y == cacti[j].thorn.y)
            {
                sbots[i].health -= 1;              // Reduce sbot health
                cacti[j].thorn.x = cacti[j].x + 1; // Reset thorn to initial position
            }
        }
    }
    // collision detection between robot and pea shooter
    for (int i = 0; i < n_robots; i++)
    {
        for (int j = 0; j < n_peashooter; j++)
        {

            if (robots[i].active && peas[j].active && robots[i].x == peas[j].x && robots[i].y == peas[j].y)
            {
                peas[j].active = false; // Remove the pea shooter if robot reaches it
            }
        }
    }
    // collision detection between sbot and cactus
    for (int i = 0; i < n_sbots; i++)
    {
        for (int j = 0; j < n_cactus; j++)
        {

            if (sbots[i].active && cacti[j].active && sbots[i].x == cacti[j].x && sbots[i].y == cacti[j].y)
            {
                cacti[j].active = false; // Remove the cactus if robot reaches it
            }
        }
    }

    // set active status
    for (int i = 0; i < n_peashooter; i++)
    {
        if (peas[i].health <= 0)
            peas[i].active = false;
    }
    for (int i = 0; i < n_robots; i++)
    {

        if (robots[i].health <= 0)
            robots[i].active = false;
    }
    for (int i = 0; i < n_cactus; i++)
    {
        if (cacti[i].health <= 0)
            cacti[i].active = false;
    }

    for (int i = 0; i < n_sbots; i++)
    {
        if (sbots[i].health <= 0)
            sbots[i].active = false;
    }
    for (int i = 0; i < n_peashooter; i++)
    {
        peas[i].bullet.active = peas[i].active;
    }
    for (int i = 0; i < n_cactus; i++)
    {
        cacti[i].thorn.active = cacti[i].active;
    }

    // Clear the grid for new frame
    for (int i = 0; i < row; i++)
    {
        for (int j = 0; j < col; j++)
        {
            grid[i][j] = ' ';
        }
    }

    // Place robot on grid if it has health
    for (int i = 0; i < n_robots; i++)
    {
        if (robots[i].active)
        {
            grid[robots[i].y][robots[i].x] = robots[i].display;
        }
    }

    // Place sbot on grid if it has health
    for (int i = 0; i < n_sbots; i++)
    {
        if (sbots[i].active)
        {
            grid[sbots[i].y][sbots[i].x] = sbots[i].display;
        }
    }
    // Place pea shooter on grid if it has health
    for (int i = 0; i < n_peashooter; i++)
    {
        if (peas[i].active)
        {
            grid[peas[i].y][peas[i].x] = peas[i].display;
        }
    }
    // Place cactus on grid if it has health
    for (int i = 0; i < n_cactus; i++)
    {
        if (cacti[i].active)
        {
            grid[cacti[i].y][cacti[i].x] = cacti[i].display;
        }
    }

    // Place bullets on grid
    for (int i = 0; i < n_peashooter; i++)
    {
        if (peas[i].bullet.active)
        {
            grid[peas[i].bullet.y][peas[i].bullet.x] = peas[i].bullet.display;
        }
    }
    // Place thorns on grid
    for (int i = 0; i < n_cactus; i++)
    {
        if (cacti[i].thorn.active)
        {
            grid[cacti[i].thorn.y][cacti[i].thorn.x] = cacti[i].thorn.display;
        }
    }
}

// Drawing the Grid
void draw()
{
    clearScreen();
    for (int y = 0; y < row; y++)
    {
        for (int x = 0; x < col; x++)
            printf("-");
        printf("\n");

        for (int x = 0; x < col; x++)
            printf("%c", grid[y][x]);
        printf("\n");
    }

    for (int x = 0; x < col; x++)
        printf("-");
    printf("\n");
}
