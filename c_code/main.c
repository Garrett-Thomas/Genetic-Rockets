#include <box2d/box2d.h>

#define RAYGUI_IMPLEMENTATION
#include <raylib.h>
#include "raygui.h"
#include "novaphysics/novaphysics.h"
#include <rlgl.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MENU_HEIGHT 100
#define DNA_SIZE 800
#define ROCKET_SIZE 25
#define NUM_ROCKETS 100
#define ROCKET_WIDTH 10
#define ROCKET_HEIGHT 30
#define SCREEN_WIDTH 700
#define SCREEN_HEIGHT 700
#define ROCKETBITS 0x000000002
#define OBSTACLEBITS 0x00000004
#define NUM_SUPER_CLONES 5
#define NUM_SURE_MUTATIONS 10
#define PERCENT_MUTATED 0.75f
#define MUTATION_SIZE 9000
#define MAX_FORCE 30000
#define LATERAL_MAX 50000
#define DEBUG 0
#define TARGET_SIZE 12
#define ROCKET_X 100
#define ROCKET_Y SCREEN_HEIGHT - GROUND_HEIGHT - (ROCKET_HEIGHT / 2)
#define GROUND_HEIGHT 20

struct Target
{
    double x;
    double y;
};
struct Rocket
{
    struct Target *targ;
    b2BodyDef bodyDef;
    b2BodyId bodyId;
    b2ShapeDef shapeDef;
    b2Polygon rectBox;

    double dna[DNA_SIZE * 2];
    int width;
    int height;
    int lifecount;
    double fitness;
};

struct Rocket **allRockets;
struct Target targ;
static int generation;
double randNum(int min, int max)
{
    return rand() % (max - min + 1) + min;
}

// Copied from SO id 22186423
void gen_random_numbers(double *array, int len, int min, int max)
{
    for (int i = 0; i < len; i++)
    {
        if (i % 2 != 0)
        {
            array[i] = randNum(min, max);
        }
        else
        {
            array[i] = randNum(-1 * LATERAL_MAX, LATERAL_MAX);
        }
    }
}
double mutateValue(double init, int mutSize, int n)
{

    // Lateral Force then
    if (n % 2 == 0)
    {
        return (double)((int)(init + randNum(-1 * mutSize, mutSize)) % LATERAL_MAX);
    }

    else
    {
        return (double)((int)(init + randNum((int)(-1.0f * (float)mutSize), mutSize)) % MAX_FORCE);
    }
}

void mutateDNA(double *arr1, double *arr2, double *arr3, int len)
{

    int midpoint = rand() % len;

    for (int i = 0; i < len; i++)
    {

        if (midpoint < i)
        {
            arr1[i] = arr2[i];
        }
        if (rand() % 30 == 0)
        {
            if (i % 2 == 0)
            {
                arr1[i] = randNum(-1 * LATERAL_MAX, LATERAL_MAX);
            }
            else
            {
                arr1[i] = randNum(-1 * MAX_FORCE, MAX_FORCE);
            }
        }
        else
        {
            arr1[i] = arr3[i];
        }
    }
}

int compareFitness(const void *rocket1, const void *rocket2)
{

    const struct Rocket *rock1 = *(const struct Rocket **)rocket1;
    const struct Rocket *rock2 = *(const struct Rocket **)rocket2;

    return (rock2->fitness - rock1->fitness < 0.f ? -1 : 1);
}

// Sort each rocket in array by fitness
// Top performer can be a clone.
// Next x percent all have dna mutated from clone
// remaining bits get new dna.
void breedNewRockets(struct Rocket **temp)
{

    qsort(allRockets, NUM_ROCKETS, sizeof(struct Rocket *), compareFitness);
    struct Rocket *rocks[NUM_ROCKETS];

    for (int i = 0; i < NUM_ROCKETS; i++)
    {

        rocks[i] = allRockets[i];
    }

    int badRocketIndex = (((NUM_ROCKETS - 1) * 3) / 4);
    for (int i = 1; i < NUM_ROCKETS; i++)
    {

        allRockets[i]->fitness = allRockets[0]->fitness / allRockets[i]->fitness;

        if (allRockets[i]->fitness < .85f)
        {
            badRocketIndex = i;
            break;
        }
    }
    allRockets[0]->fitness = 1.0f;

    for (int i = NUM_SUPER_CLONES; i < NUM_ROCKETS - NUM_SURE_MUTATIONS; i++)
    {

        mutateDNA(allRockets[i]->dna, allRockets[rand() % NUM_SUPER_CLONES]->dna, allRockets[rand() % ((i % badRocketIndex) + 1)]->dna, DNA_SIZE * 2);
        // allRockets[i]->dna = allRockets[0]->dna;
    }

    for (int i = NUM_ROCKETS - NUM_SURE_MUTATIONS; i < NUM_ROCKETS; i++)
    {

        gen_random_numbers(allRockets[i]->dna, DNA_SIZE * 2, -MAX_FORCE, MAX_FORCE);
    }
}

void rocketUpdate()
{

    for (int i = 0; i < NUM_ROCKETS; i++)
    {

        b2Vec2 pos = b2Body_GetPosition(allRockets[i]->bodyId);

        b2BodyId rockBod = allRockets[i]->bodyId;

        float angle = b2Rot_GetAngle(b2Body_GetRotation(allRockets[i]->bodyId));

        float y = sin(angle);
        float x = cos(angle);

        allRockets[i]->fitness = 1.0f / (sqrt(pow(allRockets[i]->targ->x - pos.x + (x * (ROCKET_HEIGHT / 2)), 2) + pow(allRockets[i]->targ->y - pos.y + (y * (ROCKET_HEIGHT / 2)), 2)));

        double timeValue = (1.0f / allRockets[i]->lifecount) * allRockets[i]->fitness;
        allRockets[i]->fitness += timeValue;

        int lifecount = allRockets[i]->lifecount++;

        float torque = allRockets[i]->dna[lifecount * 2];

        b2Vec2 force = {y * allRockets[i]->dna[(lifecount * 2) + 1], -1 * x * allRockets[i]->dna[(lifecount * 2) + 1]};
        b2Vec2 point = {pos.x, pos.y};

        b2Body_ApplyTorque(rockBod, torque, true);
        b2Body_ApplyForce(rockBod, force, point, true);
    }
}

void rocketDraw()
{

    for (int i = NUM_ROCKETS - 1; i >= 0; i--)
    {
        b2Vec2 pos = b2Body_GetPosition(allRockets[i]->bodyId);

        Color color = BLACK;
        if(i < NUM_SUPER_CLONES && generation > 1){
            
            color = PURPLE;

        }


        DrawRectanglePro((Rectangle){pos.x, pos.y, allRockets[i]->width, allRockets[i]->height}, (Vector2){allRockets[i]->width / 2, allRockets[i]->height / 2}, b2Rot_GetAngle(b2Body_GetRotation(allRockets[i]->bodyId)) * (180.f / PI), color);
    }
}
void drawTarget()
{



    DrawRectangle(targ.x - TARGET_SIZE , targ.y - TARGET_SIZE, TARGET_SIZE * 2, TARGET_SIZE * 2, RED);
    DrawRectangle(targ.x - (TARGET_SIZE / 2.f), targ.y - (TARGET_SIZE / 2.f), TARGET_SIZE, TARGET_SIZE, WHITE);
    DrawRectangle(targ.x - (TARGET_SIZE / 4.f), targ.y - (TARGET_SIZE / 4.f), TARGET_SIZE / 2.f, TARGET_SIZE / 2.f, RED);

}
void makeRocket(struct Rocket *rock, b2WorldId *worldId, struct Target *targ, bool makeDNA)
{

    if (makeDNA)
    {
        gen_random_numbers(rock->dna, DNA_SIZE * 2, -MAX_FORCE, MAX_FORCE);
    }
    rock->targ = targ;

    rock->bodyDef = b2DefaultBodyDef();
    rock->bodyDef.type = b2_dynamicBody;
    rock->bodyDef.position = (b2Vec2){ROCKET_X, ROCKET_Y};

    rock->bodyId = b2CreateBody(*worldId, &rock->bodyDef);

    rock->shapeDef = b2DefaultShapeDef();
    rock->shapeDef.density = 1.0f;
    rock->shapeDef.friction = 0.3f;
    rock->shapeDef.filter.categoryBits = ROCKETBITS;
    rock->shapeDef.filter.maskBits = OBSTACLEBITS;

    rock->rectBox = b2MakeBox(ROCKET_WIDTH / 2.0f, ROCKET_HEIGHT / 2.0f);
    b2CreatePolygonShape(rock->bodyId, &rock->shapeDef, &rock->rectBox);
    rock->width = ROCKET_WIDTH;
    rock->height = ROCKET_HEIGHT;
    rock->lifecount = 0;
    rock->fitness = 0.f;
}

void resetRockets(struct Target *targ, b2WorldId worldId)
{
    for (int i = 0; i < NUM_ROCKETS; i++)
    {

        allRockets[i]->fitness = 0;
        allRockets[i]->lifecount = 0;
        b2Body_SetAngularVelocity(allRockets[i]->bodyId, 0.f);
        b2Body_SetLinearVelocity(allRockets[i]->bodyId, (b2Vec2){0, 0});

        // double currAngle = b2Rot_GetAngle(b2Body_GetRotation(allRockets[i]->bodyId));
        b2Body_SetTransform(allRockets[i]->bodyId, (b2Vec2){ROCKET_X, ROCKET_Y}, b2MakeRot(0));
    }
}
void right(struct Rocket *rock)
{

    b2Vec2 pos = b2Body_GetPosition(rock->bodyId);

    b2Body_ApplyTorque(rock->bodyId, 100000, true);
}
void left(struct Rocket *rock)
{

    b2Vec2 pos = b2Body_GetPosition(rock->bodyId);

    b2Body_ApplyTorque(rock->bodyId, -100000, true);
}
void up(struct Rocket *rock)
{

    float angle = b2Rot_GetAngle(b2Body_GetRotation(rock->bodyId));

    b2Vec2 pos = b2Body_GetPosition(rock->bodyId);

    float y = sin(angle);
    float x = cos(angle);

    printf("x: %f, y:%f\n", pos.x, pos.y);
    b2Body_ApplyForce(rock->bodyId, (b2Vec2){y * 10000, x * -10000}, (b2Vec2){pos.x, pos.y}, true);
}
void drawTester(struct Rocket *rock)
{

    b2Vec2 pos = b2Body_GetPosition(rock->bodyId);

    DrawRectanglePro((Rectangle){pos.x, pos.y, rock->width, rock->height}, (Vector2){rock->width / 2, rock->height / 2}, b2Rot_GetAngle(b2Body_GetRotation(rock->bodyId)) * (180.f / PI), GREEN);
}

void setRandomTarget(struct Target *targ){

    targ->x = (rand() % (SCREEN_WIDTH - (2 * TARGET_SIZE))) + TARGET_SIZE;

    targ->y = (rand() % (SCREEN_HEIGHT - GROUND_HEIGHT - MENU_HEIGHT - TARGET_SIZE)) + MENU_HEIGHT + TARGET_SIZE ;


}


int main()
{
    nvSpace *space = nvSpace_new();
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Genetic Rockets");

    SetTargetFPS(60);

    generation = 1;

    setRandomTarget(&targ);


    b2WorldDef worldDef = b2DefaultWorldDef();
    worldDef.gravity = (b2Vec2){0.0f, 10.0f};
    b2WorldId worldId = b2CreateWorld(&worldDef);

    allRockets = malloc((NUM_ROCKETS * sizeof(struct Rocket *)) + 1);

    struct Rocket *tester = malloc(sizeof(struct Rocket));
    makeRocket(tester, &worldId, &targ, 1);

    for (int i = 0; i < NUM_ROCKETS; i++)
    {

        allRockets[i] = malloc(sizeof(struct Rocket));

        makeRocket(allRockets[i], &worldId, &targ, 1);
    }

    b2Body_SetTransform(tester->bodyId, (b2Vec2) {500, ROCKET_Y}, b2MakeRot(0));
    b2BodyDef groundBodyDef = b2DefaultBodyDef();
    groundBodyDef.position = (b2Vec2){SCREEN_WIDTH / 2, SCREEN_HEIGHT - (GROUND_HEIGHT / 2)};
    groundBodyDef.type = b2_staticBody;
    b2BodyId groundId = b2CreateBody(worldId, &groundBodyDef);
    b2Polygon groundBox = b2MakeBox(SCREEN_WIDTH, GROUND_HEIGHT / 2);

    b2ShapeDef groundShapeDef = b2DefaultShapeDef();

    groundShapeDef.filter.categoryBits = OBSTACLEBITS;

    b2CreatePolygonShape(groundId, &groundShapeDef, &groundBox);
    int count = 0;
    float speed = 2.0f;

    while (!WindowShouldClose())
    {

        if (IsKeyDown(KEY_RIGHT))
            right(tester);
        if (IsKeyDown(KEY_LEFT))
            left(tester);
        if (IsKeyDown(KEY_UP))
            up(tester);

        if (count + 1 < DNA_SIZE)
        {
            b2World_Step(worldId, 1.0f / 60.f, 4);
            rocketUpdate();

            drawTester(tester);
            if (count % (int)speed == 0)
            {

                BeginDrawing();
                ClearBackground(RAYWHITE);

                rocketDraw();
           

                DrawRectangle(0, 0, SCREEN_WIDTH, MENU_HEIGHT, GRAY);
                char buffer[50]; // Adjust size as needed
                snprintf(buffer, sizeof(buffer), "%s%d", "Generation: ", generation);

                GuiSetStyle(DEFAULT, TEXT_SIZE, 20);
                GuiSetStyle(DEFAULT, TEXT_COLOR_NORMAL, 0x33FFFF);
                GuiSliderBar((Rectangle){(SCREEN_WIDTH / 2) + 150, 20, 100, 40}, "Speed", TextFormat(" %.0fx", speed), &speed, 1, 16);    
                if(GuiButton((Rectangle){200, 20, 200, 70}, "Change Target")){

                    setRandomTarget(&targ);
                    resetRockets(&targ, worldId);
                    generation = 1;
                    count = -1;

                }
                DrawText(buffer, 20, 30, 20, ORANGE);

                DrawRectangle(0, SCREEN_HEIGHT - GROUND_HEIGHT, SCREEN_WIDTH, GROUND_HEIGHT, GetColor(0x006400ff));
     drawTarget();
                EndDrawing();
            }
        }
        else
        {
            generation++;
            breedNewRockets(allRockets);
            resetRockets(&targ, worldId);
            count = -1;
        }
        count++;
    }
    b2DestroyWorld(worldId);
    for (int i = 0; i < NUM_ROCKETS; i++)
    {
        free(allRockets[i]);
    }
    free(allRockets);
    CloseWindow();
    return 0;
}
