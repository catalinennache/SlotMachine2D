#include<windows.h>
#include <gl/freeglut.h>
#include <deque>
#include <queue>
#include <vector>
#include <string>
#include <iostream>
#include <functional>
#include <random>

using namespace std;

enum stari_posibile_pacanea { IDLE, STARTING_TO_SHUFFLE, SHUFFLING, STOPPING_FROM_SHUFFLE };
enum class tipuri_evenimente { LEVER_PULLED, STAKE_INC, STAKE_DEC };
enum class GUIComponentTypes { COMPLEX, TRIANGLE, SQUARE, CIRCLE };

struct Point {
    float x, y;
};

struct Color {
    float r, g, b;
};

class Actor {
public:
    bool isAvailable = true;
};

class GUIComponent :public Actor {

public:
    vector<GUIComponent*>* components;//subcomponents
    vector<Point>* puncte_componenta;
    float position_x;
    float position_y;
    float relative_y_position;

    float relative_rotation = 0;
    Color color;
    string label;
    GUIComponentTypes type;
    GUIComponent* parent;


    GUIComponent(float posx, float posy, string label) {
        this->components = new vector<GUIComponent*>();
        this->puncte_componenta = new vector<Point>();
        this->position_x = posx;
        this->position_y = posy;
        this->label = label;
        this->type = type;
        this->color = { 1,0,0 };
        this->type = GUIComponentTypes::COMPLEX; //by default componenta e una complexa (nu e o componenta nativa)
        this->parent = nullptr;
    }

    void changeColor(Color clr) {
        this->color = clr;
    }

    void addPoint(Point p) {
        puncte_componenta->push_back(p);

    }
    void addSubComponent(GUIComponent* cmp) {
        components->push_back(cmp);
        cmp->parent = this;
    }

    float getAbsolutePositionX() {
        float x = this->position_x;

        GUIComponent* tmp = this->parent;
        while (tmp != nullptr) {
            x += tmp->position_x;
            tmp = tmp->parent;
        }

        return x;

    }

    float getAbsolutePositionY() {
        float y = this->position_y;

        GUIComponent* tmp = this->parent;
        while (tmp != nullptr) {
            y += tmp->position_y;
            tmp = tmp->parent;
        }
        return y;
    }
    

    float* getParentAbsolutePosition() {
        float x = 0;
        float y = 0;

        GUIComponent* tmp = this->parent;
        while (tmp != nullptr) {
            x += tmp->position_x;
            y += tmp->position_y;
            tmp = tmp->parent;
        }

        return new float[] {x, y};

    }


};

class Rectang : public GUIComponent {
public:
    Rectang(float posx, float posy, Point p1, Point p2, Point p3, Point p4, string label, Color clr) :GUIComponent(posx, posy, label) {
        this->color = clr;
        this->puncte_componenta->push_back(p1);
        this->puncte_componenta->push_back(p2);
        this->puncte_componenta->push_back(p3);
        this->puncte_componenta->push_back(p4);
        this->type = GUIComponentTypes::SQUARE;
    }

};

class Triangle : public GUIComponent {
public:
    Triangle(float posx, float posy, Point p1, Point p2, Point p3, string label, Color clr) :GUIComponent(posx, posy, label) {
        this->color = clr;
        this->puncte_componenta->push_back(p1);
        this->puncte_componenta->push_back(p2);
        this->puncte_componenta->push_back(p3);
        this->type = GUIComponentTypes::TRIANGLE;
    }

};

class Circle : public GUIComponent {
public:
    float radius;

    float resolution;
    Circle(float posx, float posy, float radius, float resolution, string label, Color clr) :GUIComponent(posx, posy, label) {
        this->radius = radius;
        this->resolution = resolution;
        this->color = clr;
        this->type = GUIComponentTypes::CIRCLE;
    }



};

class Symbol {
public:
    GUIComponentTypes type;
    int column;
    Actor* current_used_guicomponent = nullptr;
    bool markedForDeletion = false;

    Symbol(GUIComponentTypes type, int column) {
        this->type = type;
        this->column = column;
    }



    void disposeInnerGUIComponent() {
        this->current_used_guicomponent->isAvailable = true;
        ((GUIComponent*)this->current_used_guicomponent)->parent = nullptr;
        ((GUIComponent*)this->current_used_guicomponent)->relative_y_position = 0;
        ((GUIComponent*)this->current_used_guicomponent)->position_y = 1300;
        this->current_used_guicomponent = nullptr;
        this->markedForDeletion = true;
    }

};


Color generateColor() {
    float r = 1.0 * (rand() % 10);
    float g = 1.0 * (rand() % 10);
    float b = 1.0 * (rand() % 10);

    return { r,g,b };

}

GUIComponentTypes generateNewSymbol() {
    //random generator to implement
    int random_index = rand() % 3;
    GUIComponentTypes types[3] = { GUIComponentTypes::TRIANGLE, GUIComponentTypes::CIRCLE, GUIComponentTypes::SQUARE };

    return types[random_index];
}



class MonitorColumn : public GUIComponent {
public:
    deque<Symbol*>* components;
    vector<Actor*>* actors;
    float distance_between_symbols = 100;
    float relative_speed = 0.1;
    float height = 400;
    float width = 150;
    float column = 0;
    MonitorColumn(float posx, float posy, string label, int column) :GUIComponent(posx, posy, label) {
        components = new deque<Symbol*>();
        actors = new vector<Actor*>();
        this->column = column;
    }

    void init() {
        this->actors = generateActorSetFor();
        this->components->push_front(nullptr);
        this->addSymbol(generateNewSymbol());

        this->components->push_front(nullptr);
        this->addSymbol(generateNewSymbol());

        this->components->push_front(nullptr);
        this->addSymbol(generateNewSymbol());

        this->components->push_front(nullptr);
        this->addSymbol(generateNewSymbol());
    }

    void addSymbol(GUIComponentTypes type) {
        int free_actor_index = -1;
        for (int i = 0; i < actors->size(); i++) {
            Actor* ca = actors->at(i);

            if (ca->isAvailable && ((GUIComponent*)ca)->type == type) {
                free_actor_index = i;
                ca->isAvailable = false;
                break;
            }
        }
        if (free_actor_index != -1) {
            Symbol* symbol = new Symbol(type, column);
            symbol->current_used_guicomponent = actors->at(free_actor_index);
            ((GUIComponent*)symbol->current_used_guicomponent)->position_x = 0;
            ((GUIComponent*)symbol->current_used_guicomponent)->position_y = 0;
            ((GUIComponent*)symbol->current_used_guicomponent)->parent = this;
            components->at(0) = symbol;
        }
    }


    vector<Actor*>* generateActorSetFor() {
        MonitorColumn* mc = this;
        float absX = mc->getAbsolutePositionX();
        float hidePosition = 1300;
        vector<Actor*>* tmp = new vector<Actor*>();
        GUIComponent* actor_tmp = new Triangle(absX, hidePosition, { 0,0 }, { 40,0 }, { 20,40 }, "actor_0", { 1,0,1 });
        tmp->push_back((Actor*)actor_tmp);
        actor_tmp = new Triangle(absX, hidePosition, { 0,0 }, { 40,0 }, { 20,40 }, "actor_1", { 1,0,1 });
        tmp->push_back((Actor*)actor_tmp);

        actor_tmp = new Triangle(absX, hidePosition, { 0,0 }, { 40,0 }, { 20,40 }, "actor_2", { 1,0,1 });
        tmp->push_back((Actor*)actor_tmp);

        actor_tmp = new Triangle(absX, hidePosition, { 0,0 }, { 40,0 }, { 20,40 }, "actor_3", { 1,0,1 });
        tmp->push_back((Actor*)actor_tmp);

        actor_tmp = new Rectang(absX, hidePosition, { 0,0 }, { 40,0 }, { 40,40 }, { 0,40 }, "actor_4", { 0,0,1 });
        tmp->push_back((Actor*)actor_tmp);

        actor_tmp = new Rectang(absX, hidePosition, { 0,0 }, { 40,0 }, { 40,40 }, { 0,40 }, "actor_5", { 0,0,1 });
        tmp->push_back((Actor*)actor_tmp);

        actor_tmp = new Rectang(absX, hidePosition, { 0,0 }, { 40,0 }, { 40,40 }, { 0,40 }, "actor_6", { 0,0,1 });
        tmp->push_back((Actor*)actor_tmp);

        actor_tmp = new Rectang(absX, hidePosition, { 0,0 }, { 40,0 }, { 40,40 }, { 0,40 }, "actor_7", { 0,0,1 });
        tmp->push_back((Actor*)actor_tmp);

        actor_tmp = new Circle(absX, hidePosition, 20, 10, "actor_8", { 0,0,1 });
        tmp->push_back((Actor*)actor_tmp);

        actor_tmp = new Circle(absX, hidePosition, 20, 10, "actor_9", { 0,0,1 });
        tmp->push_back((Actor*)actor_tmp);

        actor_tmp = new Circle(absX, hidePosition, 20, 10, "actor_10", { 0,0,1 });
        tmp->push_back((Actor*)actor_tmp);

        actor_tmp = new Circle(absX, hidePosition, 20, 10, "actor_11", { 0,0,1 });
        tmp->push_back((Actor*)actor_tmp);

        return tmp;

    }


    void shiftSymbols(float new_rspeed) {
        this->relative_speed = new_rspeed;
    }

    float computeYVector(int s_index) {
        return this->relative_speed;
    }

    float computeAbsY(int s_index) {

        return getParentAbsolutePosition()[1] + s_index * distance_between_symbols;
    }

    float computeRelativeYPos(int s_index) {
        GUIComponent* current_comp = (GUIComponent*)components->at(s_index)->current_used_guicomponent;
        return current_comp->position_y / height;
    }

    float computeCurrentScale(int s_index) {
        float relative_y_pos = computeRelativeYPos(s_index) * 100;
        if (relative_y_pos < 40 || relative_y_pos > 60)
            return 1.0; //no maginification

        return 1 + (-0.008 * (relative_y_pos - 50) * (relative_y_pos - 50) + 0.75); //-0.008*(x-50)^2 + 0.75
    }

};



class GUIManager {
public:
    vector<GUIComponent*>* lights;
    vector<GUIComponent*>* console_components;
    GUIComponent* lever;
    GUIComponent* monitor;
    vector<GUIComponent*>* masks;

    float disposalThreshold = 575;

    void drawComponent(GUIComponent* gcomp, string ignored_component = "") {
        if (gcomp->label.compare(ignored_component) == 0) {
            return;
        }

        if (gcomp->type == GUIComponentTypes::CIRCLE) {
            Circle* circle = (Circle*)gcomp;
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            glBegin(GL_POLYGON);
            for (int ii = 0; ii < circle->resolution; ii++)
            {
                float theta = 2.0f * 3.1415926f * float(ii) / float(circle->resolution);
                glColor3f(circle->color.r, circle->color.g, circle->color.b);
                float x = circle->radius * cosf(theta);
                float y = circle->radius * sinf(theta);
                float positionX = circle->getAbsolutePositionX();
                float positionY = circle->getAbsolutePositionY();
                glVertex2f(x + positionX, y + positionY);
            }
            glEnd();
        }
        else if (gcomp->type == GUIComponentTypes::COMPLEX) {
            for (int i = 0; i < gcomp->components->size(); i++) {
                drawComponent(gcomp->components->at(i), ignored_component);
            }
        }
        else {
            glBegin(GL_POLYGON);
            float offsetX = gcomp->getAbsolutePositionX();
            float offsetY = gcomp->getAbsolutePositionY();
            for (int i = 0; i < gcomp->puncte_componenta->size(); i++) {
                Point p = gcomp->puncte_componenta->at(i);
                glColor3f(gcomp->color.r, gcomp->color.g, gcomp->color.b);
                glVertex2f(offsetX + p.x, offsetY + p.y);
            }
            glEnd();
        }

    }

    void moveComponent(GUIComponent* gcomp, float x, float y, float scale) {

        glPushMatrix();
        gcomp->position_x = x;
        gcomp->position_y = y;

        if (scale > 1) {
            float pX = gcomp->getAbsolutePositionX();
            float pY = gcomp->getAbsolutePositionY();

            glTranslatef(pX, pY, 0);

            glScalef(scale, scale, 1);
            if (gcomp->type != GUIComponentTypes::CIRCLE) {
                glTranslatef(-pX, -pY, 0);
                glTranslatef(-scale * 4, -scale * 4, 0);
            }
            else
                glTranslatef(-pX, -pY, 0);
        }
        else {

        }
        this->drawComponent(gcomp);
        glPopMatrix();
    }



    void drawSymbol(MonitorColumn* m, int symbol_index) {
        Symbol* s = m->components->at(symbol_index);
        GUIComponent* physical_component = (GUIComponent*)s->current_used_guicomponent;
        if (physical_component == nullptr)
            return;
        float x = physical_component->type == GUIComponentTypes::CIRCLE ? 20 : 0;
        float y = m->distance_between_symbols * symbol_index + physical_component->relative_y_position + m->computeYVector(symbol_index);

        physical_component->relative_y_position += m->computeYVector(symbol_index);

        float scale = m->computeCurrentScale(symbol_index);
        this->moveComponent(physical_component, x, y, scale); 

    }


    void rotateComponent(GUIComponent* gcmp) {
        gcmp->relative_rotation += 2;
        float posX = gcmp->getAbsolutePositionX();
        float posY = gcmp->getAbsolutePositionY();
        glPushMatrix();
        glTranslated(posX, posY, 0);
        glRotatef(gcmp->relative_rotation, 0.0f, 0.0f, 1.0f);   //X
        glTranslated(-posX, -posY, 0);
        this->drawComponent(gcmp);
        glPopMatrix();

    }

    void translateComponent(GUIComponent* gcmp, float x, float y) {
        glPushMatrix();
        glTranslated(x, y, 0);
        this->drawComponent(gcmp);
        glPopMatrix();
    }

};

GUIManager* guiManager = new GUIManager();

void paintMonitorFrame() {
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glBegin(GL_POLYGON);
    glColor3f(0, 0, 0);
    glVertex2f(100, 230);

    glColor3f(0, 0, 0);
    glVertex2f(500, 230);

    glColor3f(0, 0, 0);
    glVertex2f(500, 520);


    glColor3f(0, 0, 0);
    glVertex2f(100, 520);

    glEnd();
}

GUIComponent* generateMonitor() {
    GUIComponent* wrapper = new GUIComponent(150, 175, "monitor");

    MonitorColumn* column1 = new MonitorColumn(0, 0, "monitor_column1", 0);
    wrapper->addSubComponent(column1);

    column1->init();

    MonitorColumn* column2 = new MonitorColumn(125, 0, "monitor_column2", 0);
    wrapper->addSubComponent(column2);

    column2->init();

    MonitorColumn* column3 = new MonitorColumn(250, 0, "monitor_column2", 0);
    wrapper->addSubComponent(column3);

    column3->init();

    return wrapper;
}

vector<GUIComponent*>* generateMasks() {
    vector<GUIComponent*>* tmp = new vector<GUIComponent*>();

    GUIComponent* ctmp = new Rectang(100, 520, { 0,0 }, { 425,0 }, { 425,500 }, { 0,500 }, "upper_p", { 1,1,1 });
    tmp->push_back(ctmp);

    ctmp = new Rectang(100, 80, { 0,0 }, { 425,0 }, { 425,150 }, { 0,150 }, "lower_p", { 1,1,1 });
    tmp->push_back(ctmp);

    return tmp;

}

vector<GUIComponent*>* generateLightComponents() {
    vector<GUIComponent*>* temp = new vector<GUIComponent*>();

    temp->push_back(new Circle(25, 50, 20, 100, "light1", { 1,1,0 }));
    temp->push_back(new Circle(25, 150, 20, 100, "light2", { 1,1,0 }));

    temp->push_back(new Circle(25, 250, 20, 100, "light3", { 1,1,0 }));
    temp->push_back(new Circle(25, 350, 20, 100, "light3", { 1,1,0 }));
    temp->push_back(new Circle(25, 450, 20, 100, "light3", { 1,1,0 }));
    temp->push_back(new Circle(25, 550, 20, 100, "light3", { 1,1,0 }));

    temp->push_back(new Circle(125, 50, 20, 100, "light4", { 1,1,0 }));
    temp->push_back(new Circle(225, 50, 20, 100, "light4", { 1,1,0 }));
    temp->push_back(new Circle(325, 50, 20, 100, "light4", { 1,1,0 }));
    temp->push_back(new Circle(425, 50, 20, 100, "light4", { 1,1,0 }));
    temp->push_back(new Circle(525, 50, 20, 100, "light4", { 1,1,0 }));
    temp->push_back(new Circle(625, 50, 20, 100, "light4", { 1,1,0 }));
    temp->push_back(new Circle(725, 50, 20, 100, "light4", { 1,1,0 }));


    return temp;

}


GUIComponent* generateLeverComponent() {
    GUIComponent* lever = new GUIComponent(650, 150, "lever");
    Rectang* r1 = new Rectang(0, 0, { 0,0 }, { 75,0 }, { 75,400 }, { 0,400 }, "lever_base", { 0,0,0 });
    lever->addSubComponent(r1);

    Rectang* r2 = new Rectang(0, 0, { 10,10 }, { 65,10 }, { 65,390 }, { 10,390 }, "lever_inner", { 1,0,1 });
    lever->addSubComponent(r2);

    GUIComponent* tmp = new GUIComponent(37.5, 370, "knob_wrp");
    Circle* c1 = new Circle(0, 0, 50, 100, "lever_knob", { 1,0,0 });
    tmp->addSubComponent(c1);


    Triangle* triunghi_stea_1 = new Triangle(-9, -8, { 0, 0 }, { 20,0 }, { 10,20 }, "triunghi_stea_1", { 1,1,1 });
    tmp->addSubComponent(triunghi_stea_1);

    Triangle* triunghi_stea_2 = new Triangle(-9, -8, { 0, 16 }, { 20,16 }, { 10,-5 }, "triunghi_stea_2", { 1,1,1 });
    tmp->addSubComponent(triunghi_stea_2);


    lever->addSubComponent(tmp);
    return lever;
}



void shiftElementsRight(MonitorColumn* mc) {

    deque<Symbol*>* v = mc->components;
    Symbol* symbol_to_be_deleted = v->at(v->size() - 1);
    for (int i = v->size() - 2; i >= 0; i--) {
        v->at(i + 1) = v->at(i);
        ((GUIComponent*)v->at(i)->current_used_guicomponent)->relative_y_position = 0;
    }

    ((GUIComponent*)symbol_to_be_deleted->current_used_guicomponent)->relative_y_position = 0;
    symbol_to_be_deleted->disposeInnerGUIComponent();

    mc->addSymbol(generateNewSymbol());
}

void paintSymbols(float speed) {

    for (int i = 0; i < guiManager->monitor->components->size(); i++) {
        MonitorColumn* mc = (MonitorColumn*)guiManager->monitor->components->at(i);
        int r = rand() % 4;
        if (speed == 0) {
            r = 0;
        }

        mc->shiftSymbols(speed+r);
        for (int i = 0; i < mc->components->size(); i++) {
            guiManager->drawSymbol(mc, i);
        }

        for (int i = 0; i < mc->components->size(); i++) {
            GUIComponent* comp = (GUIComponent*)mc->components->at(i)->current_used_guicomponent;
            if (comp == nullptr)
                continue;
            float absPosY = comp->getAbsolutePositionY();
            
            if (absPosY > guiManager->disposalThreshold) {
                shiftElementsRight(mc);
            }
        }
    }
}


void cleanMonitorColumn(MonitorColumn* mc) {
    
    for (int i = 0; i < mc->components->size(); i++) {
        if (mc->components->at(i) == nullptr || mc->components->at(i)->current_used_guicomponent == nullptr || mc->components->at(i)->markedForDeletion) {
            mc->addSymbol(generateNewSymbol());
        }
    }

}


class Game {
public:

    stari_posibile_pacanea current_state;

    Game() {
        this->current_state = IDLE;
    }



    void start() {
        guiManager->lights = generateLightComponents();
        guiManager->lever = generateLeverComponent();
        guiManager->masks = generateMasks();
        guiManager->monitor = generateMonitor();

    }
    
    

    void deseneaza() {
        
        for (int i = 0; i < 3; i++) {
            cleanMonitorColumn((MonitorColumn*)guiManager->monitor->components->at(i));
        }

        for (int i = 0; i < guiManager->lights->size(); i++) {

            if (current_state == SHUFFLING)
                guiManager->lights->at(i)->changeColor(generateColor());

            if (current_state == STARTING_TO_SHUFFLE) {
                GUIComponent* nuca = guiManager->lever->components->at(2);
                float procent = (-1.0 * nuca->relative_y_position) / 40;

                if (i > procent)
                    guiManager->lights->at(i)->changeColor({ 1,1,0 });
                else
                    guiManager->lights->at(i)->changeColor({ 1,0,0 });
            }

            if (current_state == IDLE || current_state == STOPPING_FROM_SHUFFLE)
                guiManager->lights->at(i)->changeColor({ 1,1,0 });

            guiManager->drawComponent(guiManager->lights->at(i));

        }

        guiManager->drawComponent(guiManager->lever, "knob_wrp");


        if (this->current_state == STARTING_TO_SHUFFLE) {
            GUIComponent* nuca = guiManager->lever->components->at(2);
            float posY_nuca = nuca->getAbsolutePositionY();
            if (posY_nuca + nuca->relative_y_position > 200) {
                nuca->relative_y_position -= 2;
                
                guiManager->translateComponent(nuca, 0, nuca->relative_y_position);
            }
            else {
                this->current_state = SHUFFLING;
                nuca->position_y += nuca->relative_y_position;
                nuca->relative_y_position = 0;
            }

            paintSymbols(0);


        }
        else if (current_state == IDLE) {
            GUIComponent* nuca = guiManager->lever->components->at(2);
            guiManager->drawComponent(nuca);
            paintSymbols(0);
        }
        else if (current_state == STOPPING_FROM_SHUFFLE) {
            GUIComponent* nuca = guiManager->lever->components->at(2);
            float posY_nuca = nuca->getAbsolutePositionY();
            if (posY_nuca + nuca->relative_y_position < 550) {
                nuca->relative_y_position += 2;
              
                guiManager->translateComponent(nuca, 0, nuca->relative_y_position);
            }
            else {
                this->current_state = IDLE;
                nuca->position_y += nuca->relative_y_position;
                nuca->relative_y_position = 0;
            }

            paintSymbols(0.5);

        }



        if (this->current_state == SHUFFLING) {
            guiManager->rotateComponent(guiManager->lever->components->at(2));
            paintSymbols(1);
        }



        for (int i = 0; i < guiManager->masks->size(); i++) {
            guiManager->drawComponent(guiManager->masks->at(i));
        }

        paintMonitorFrame();
    }




};


Game* game = new Game();

double i = 0.0;
double j = 0.0;
double alpha = 1.0;

void init(void)
{
    glClearColor(1.0, 1.0, 1.0, 0.0);
    glMatrixMode(GL_PROJECTION);
    glOrtho(-20.0, 1500.0, 0.0, 1500.0, -1.0, 1.0);

}


void reshape(int w, int h)
{
    glViewport(0, 0, (GLsizei)w, (GLsizei)h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-20.0, 780.0, 0.0, 600.0, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}


void keyboard(unsigned char key, int x, int y) {
    if (key == ' ') {

        if (game->current_state == IDLE) {
            game->current_state = STARTING_TO_SHUFFLE;
        }

        if (game->current_state == SHUFFLING) {
            game->current_state = STOPPING_FROM_SHUFFLE;
        }

    }


}


void deseneazaScena(void)
{
    glClear(GL_COLOR_BUFFER_BIT);
    game->deseneaza();
    glutSwapBuffers();
    glFlush();
}

void idle() {
    glutPostRedisplay();
}


int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(800, 600);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Proiect2D - Dima & Enache");
    init();
    glutDisplayFunc(deseneazaScena);
    glutReshapeFunc(reshape);
    glutIdleFunc(idle);
    glutKeyboardFunc(keyboard);
    game->start();
    glutMainLoop();
}