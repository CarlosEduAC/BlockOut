#include <GL/glew.h>
#include <GL/glu.h>
#include "glm/glm.hpp"
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/intersect.hpp>
#include "stdio.h"
#include "PIG.h"

#define ALT 8       //Quantidade de linhas do po�o (divis�es verticais)
#define LARG 8      //Quantidade de colunas do po�o (divis�es horizontais)
#define PROF 10     //Quantidade de niveis do po�o (divis�es no eixo Z)
#define LADO 0.16   //Medida de cada peda�o do po�o

//Matriz tridimensional que representa o po�o do jogo. Onde possui 0 n�o existe um bloco, e 1 indica que existe.
int poco[LARG][ALT][PROF] = {0},endgame = 0,score = 0;//Endgame usado para definir se o jogo acabou ou n�o
//Um bloco corresponde a uma parte de uma pe�a inteira
typedef struct{
    int x,y,z;
}Bloco;
//Uma pe�a � composta por 4 blocos, sendo o �ltimo bloco considerado o bloco piv�
//A posi��o real do bloco piv� em rela��o ao po�o � dado pelo valor dos campos X,Y,Z
typedef struct{
    int x,y,z;
    Bloco bl[4];
}Peca;
//Desenha as grades do po�o
void DesenhaPoco(){
    float baseY = (ALT/2)*LADO;
    float baseX = (LARG/2)*LADO;
    float baseZ = -(PROF)*LADO;
    glColor3f(0.13,0.5,0.13);
    glBegin(GL_LINES);
    //Lado esquerdo
    for (int i=0;i<=ALT;i++){
        glVertex3f(-baseX,baseY-i*LADO,0);
        glVertex3f(-baseX,baseY-i*LADO,baseZ);
    }
    for (int i=0;i<=PROF;i++){
        glVertex3f(-baseX,baseY,baseZ+i*LADO);
        glVertex3f(-baseX,-baseY,baseZ+i*LADO);
    }
    //Lado direito
    for (int i=0;i<=ALT;i++){
        glVertex3f(baseX,-i*LADO+baseY,0);
        glVertex3f(baseX,-i*LADO+baseY,baseZ);
    }
    for (int i=0;i<=PROF;i++){
        glVertex3f(baseX,baseY,baseZ+i*LADO);
        glVertex3f(baseX,-baseY,baseZ+i*LADO);
    }
    //Lado cima
    for (int i=0;i<=LARG;i++){
        glVertex3f(baseX-i*LADO,baseY,0);
        glVertex3f(baseX-i*LADO,baseY,baseZ);
    }
    for (int i=0;i<=PROF;i++){
        glVertex3f(baseX,baseY,baseZ+i*LADO);
        glVertex3f(-baseX,baseY,baseZ+i*LADO);
    }
    //Lado baixo
    for (int i=0;i<=LARG;i++){
        glVertex3f(baseX-i*LADO,-baseY,0);
        glVertex3f(baseX-i*LADO,-baseY,baseZ);
    }
    for (int i=0;i<=PROF;i++){
        glVertex3f(baseX,-baseY,baseZ+i*LADO);
        glVertex3f(-baseX,-baseY,baseZ+i*LADO);
    }
    //Fundo
    for (int i=0;i<=ALT;i++){
        glVertex3f(-baseX,baseY-i*LADO,baseZ);
        glVertex3f(baseX,baseY-i*LADO,baseZ);
    }
    for (int i=0;i<=LARG;i++){
        glVertex3f(baseX-i*LADO,baseY,baseZ);
        glVertex3f(baseX-i*LADO,-baseY,baseZ);
    }
    glEnd();
}
//Desenha um bloco (cubo) j� fixado no po�o. O n�vel � respons�vel pela mudan�a de cor entre n�veis.
void DesenhaCubo(float x,float y,float z,int nivel){
    switch(nivel){
    case 0: glColor3f(1,0,0);break;
    case 1: glColor3f(1,0,1);break;
    case 2: glColor3f(0,0,1);break;
    case 3: glColor3f(0.5,0.5,1);break;
    case 4: glColor3f(1,0.5,0.5);break;
    case 5: glColor3f(1,0,0.5);break;
    case 6: glColor3f(1,1,0);break;
    case 7: glColor3f(1,0.5,0);break;
    case 8: glColor3f(0,0.5,1);break;
    case 9: glColor3f(0.5,0,1);break;
    }
    glPushMatrix();
    glTranslatef(x,y,z);
    glBegin(GL_QUADS);
    //Frente
    glVertex3f(0,0,LADO);
    glVertex3f(0,LADO,LADO);
    glVertex3f(LADO,LADO,LADO);
    glVertex3f(LADO,0,LADO);
    //Tr�s
    glVertex3f(0,0,0);
    glVertex3f(LADO,0,0);
    glVertex3f(LADO,LADO,0);
    glVertex3f(0,LADO,0);
    //Direita
    glVertex3f(LADO,0,0);
    glVertex3f(LADO,LADO,0);
    glVertex3f(LADO,LADO,LADO);
    glVertex3f(LADO,0,LADO);
    //Esquerda
    glVertex3f(0,0,0);
    glVertex3f(0,LADO,0);
    glVertex3f(0,LADO,LADO);
    glVertex3f(0,0,LADO);
    //Cima
    glVertex3f(0,LADO,0);
    glVertex3f(LADO,LADO,0);
    glVertex3f(LADO,LADO,LADO);
    glVertex3f(0,LADO,LADO);
    //Baixo
    glVertex3f(0,0,0);
    glVertex3f(LADO,0,0);
    glVertex3f(LADO,0,LADO);
    glVertex3f(0,0,LADO);

    glEnd();
    glPopMatrix();
}
//Desenha os blocos (cubos) que j� est�o fixados no po�o
void DesenhaBlocos(){
    for (int i=0;i<LARG;i++){
        for (int j=0;j<ALT;j++){
            for (int k=0;k<PROF;k++){
                if (poco[i][j][k]){
                    DesenhaCubo((i-LARG/2)*LADO,(j-ALT/2)*LADO,-(k+1)*LADO,k);
                }
            }
        }
    }
}
//Calcula quantos blocos existem no po�o em uma dada profundidade
int QtdBlocosNaProfundidade(int prof){
    int resp = 0;
    for (int i=0;i<LARG;i++){
        for (int j=0;j<ALT;j++){
            if (poco[i][j][prof])
                resp++;
        }
    }
    return resp;
}
//Remove todos os blocos do po�o em uma dada profundidade
void RemoveProfundidade(int prof){
    if (prof==0){
        printf("Nao pode remover a profundida 0 (mais no topo do po�o)\n");
        return;
    }
    for (int k=prof;k>0;k--){
        for (int i=0;i<LARG;i++){
            for (int j=0;j<ALT;j++){
                poco[i][j][k] = poco[i][j][k-1];
            }
        }
    }
}
//Desenha uma pe�a por meio dos blocos individuais
void DesenhaPeca(Peca peca){
    glColor3f(1,1,1);
    for (int i=0;i<4;i++){
        glPushMatrix();
        glTranslatef((peca.x+peca.bl[i].x-LARG/2)*LADO,(peca.y+peca.bl[i].y-ALT/2)*LADO,-(peca.z+peca.bl[i].z)*LADO);
        //Parte de cima do cubo
        glBegin(GL_LINE_LOOP);
        glVertex3f(0,0,-LADO);
        glVertex3f(LADO,0,-LADO);
        glVertex3f(LADO,LADO,-LADO);
        glVertex3f(0,LADO,-LADO);
        glEnd();
        //Parte de baixo do cubo
        glBegin(GL_LINE_LOOP);
        glVertex3f(0,0,0);
        glVertex3f(LADO,0,0);
        glVertex3f(LADO,LADO,0);
        glVertex3f(0,LADO,0);
        glEnd();

        glBegin(GL_LINES);
        glVertex3f(0,0,0);
        glVertex3f(0,0,-LADO);
        glVertex3f(LADO,0,0);
        glVertex3f(LADO,0,-LADO);
        glVertex3f(LADO,LADO,0);
        glVertex3f(LADO,LADO,-LADO);
        glVertex3f(0,LADO,0);
        glVertex3f(0,LADO,-LADO);
        glEnd();
        glPopMatrix();
    }
}
//Muda os valores X,Y do bloco em rela��o ao bloco piv� da pe�a. O �ngulo de rota��o � de 90 graus
Bloco RodaAntiHorarioZ(Bloco bl){
    Bloco resp;
    resp.x = bl.x + (bl.x * -1) - bl.y;
    resp.y = bl.y + (bl.y * -1) + bl.x;
    resp.z = bl.z;
    return resp;
}
//Muda os valores X,Y do bloco em rela��o ao bloco piv� da pe�a. O �ngulo de rota��o � de -90 graus
Bloco RodaHorarioZ(Bloco bl){
    Bloco resp;
    resp.x = bl.x + (bl.x * -1) + bl.y;
    resp.y = bl.y + (bl.y * -1) - bl.x;
    resp.z = bl.z;
    return resp;
}
//Muda os valores Y,Z do bloco em rela��o ao bloco piv� da pe�a. O �ngulo de rota��o � de 90 graus
Bloco RodaAntiHorarioX(Bloco bl){
    Bloco resp;
    resp.z = bl.z + (bl.z * -1) - bl.y;
    resp.y = bl.y + (bl.y * -1) + bl.z;
    resp.x = bl.x;
    return resp;
}
//Muda os valores Y,Z do bloco em rela��o ao bloco piv� da pe�a. O �ngulo de rota��o � de -90 graus
Bloco RodaHorarioX(Bloco bl){
    Bloco resp;
    resp.z = bl.z + (bl.z * -1) + bl.y;
    resp.y = bl.y + (bl.y * -1) - bl.z;
    resp.x = bl.x;
    return resp;
}
//Muda os valores X,Z do bloco em rela��o ao bloco piv� da pe�a. O �ngulo de rota��o � de 90 graus
Bloco RodaAntiHorarioY(Bloco bl){
    Bloco resp;
    resp.z = bl.z + (bl.z * -1) - bl.x;
    resp.x = bl.x + (bl.x * -1) + bl.z;
    resp.y = bl.y;
    return resp;
}
//Muda os valores X,Z do bloco em rela��o ao bloco piv� da pe�a. O �ngulo de rota��o � de -90 graus
Bloco RodaHorarioY(Bloco bl){
    Bloco resp;
    resp.z = bl.z + (bl.z * -1) + bl.x;
    resp.x = bl.x + (bl.x * -1) - bl.z;
    resp.y = bl.y;
    return resp;
}
//Tenta girar uma pe�a no eixo X, no sentido antihor�rio
void TentaRodarAntiHorarioX(Peca &peca){
    int erro=0;
    //Verifica se a pe�a pode ser rotacionada, se erro igual a 1 ent�o n�o pode
    for (int i=0;i<3&&erro==0;i++){
        Bloco aux = RodaAntiHorarioX(peca.bl[i]);
        if (peca.x+aux.x<0||peca.x+aux.x>=LARG||
            peca.y+aux.y<0||peca.y+aux.y>=ALT||
            peca.z+aux.z>=PROF || poco[peca.x+aux.x][peca.y+aux.y][peca.z+aux.z]!=0)

            erro = 1;
    }
    //Se a pe�a pode ser rotacionada, ent�o rotacionamos os blocos que n�o s�o pivores
    if (!erro){
        for (int i=0;i<3&&erro==0;i++){
            peca.bl[i] = RodaAntiHorarioX(peca.bl[i]);
        }
    }
}
//Tenta girar uma pe�a no eixo X, no sentido hor�rio
void TentaRodarHorarioX(Peca &peca){
    int erro=0;
    //Verifica se a pe�a pode ser rotacionada, se erro igual a 1 ent�o n�o pode
    for (int i=0;i<3&&erro==0;i++){
        Bloco aux = RodaHorarioX(peca.bl[i]);
        if (peca.x+aux.x<0||peca.x+aux.x>=LARG||
            peca.y+aux.y<0||peca.y+aux.y>=ALT||
            peca.z+aux.z>=PROF || poco[peca.x+aux.x][peca.y+aux.y][peca.z+aux.z]!=0)

            erro = 1;
    }
    //Se a pe�a pode ser rotacionada, ent�o rotacionamos os blocos que n�o s�o pivores
    if (!erro){
        for (int i=0;i<3&&erro==0;i++){
            peca.bl[i] = RodaHorarioX(peca.bl[i]);
        }
    }
}
//Tenta girar uma pe�a no eixo Y, no sentido antihor�rio
void TentaRodarAntiHorarioY(Peca &peca){
    int erro=0;
    //Verifica se a pe�a pode ser rotacionada, se erro igual a 1 ent�o n�o pode
    for (int i=0;i<3&&erro==0;i++){
        Bloco aux = RodaAntiHorarioY(peca.bl[i]);
        if (peca.x+aux.x<0||peca.x+aux.x>=LARG||
            peca.y+aux.y<0||peca.y+aux.y>=ALT||
            peca.z+aux.z>=PROF || poco[peca.x+aux.x][peca.y+aux.y][peca.z+aux.z]!=0)

            erro = 1;
    }
    //Se a pe�a pode ser rotacionada, ent�o rotacionamos os blocos que n�o s�o pivores
    if (!erro){
        for (int i=0;i<3&&erro==0;i++){
            peca.bl[i] = RodaAntiHorarioY(peca.bl[i]);
        }
    }
}
//Tenta girar uma pe�a no eixo Y, no sentido hor�rio
void TentaRodarHorarioY(Peca &peca){
    int erro=0;
    //Verifica se a pe�a pode ser rotacionada, se erro igual a 1 ent�o n�o pode
    for (int i=0;i<3&&erro==0;i++){
        Bloco aux = RodaHorarioY(peca.bl[i]);
        if (peca.x+aux.x<0||peca.x+aux.x>=LARG||
            peca.y+aux.y<0||peca.y+aux.y>=ALT||
            peca.z+aux.z>=PROF || poco[peca.x+aux.x][peca.y+aux.y][peca.z+aux.z]!=0)

            erro = 1;
    }
    //Se a pe�a pode ser rotacionada, ent�o rotacionamos os blocos que n�o s�o pivores
    if (!erro){
        for (int i=0;i<3&&erro==0;i++){
            peca.bl[i] = RodaHorarioY(peca.bl[i]);
        }
    }
}
//Tenta girar uma pe�a no eixo Z, no sentido antihor�rio
void TentaRodarAntiHorarioZ(Peca &peca){
    int erro=0;
    //Verifica se a pe�a pode ser rotacionada, se erro igual a 1 ent�o n�o pode
    for (int i=0;i<3&&erro==0;i++){
        Bloco aux = RodaAntiHorarioZ(peca.bl[i]);
        if (peca.x+aux.x<0||peca.x+aux.x>=LARG||
            peca.y+aux.y<0||peca.y+aux.y>=ALT||
            peca.z+aux.z>=PROF || poco[peca.x+aux.x][peca.y+aux.y][peca.z+aux.z]!=0)

            erro = 1;
    }
    //Se a pe�a pode ser rotacionada, ent�o rotacionamos os blocos que n�o s�o pivores
    if (!erro){
        for (int i=0;i<3&&erro==0;i++){
            peca.bl[i] = RodaAntiHorarioZ(peca.bl[i]);
        }
    }
}
//Tenta girar uma pe�a no eixo Z, no sentido hor�rio
void TentaRodarHorarioZ(Peca &peca){
    int erro=0;
    //Verifica se a pe�a pode ser rotacionada, se erro igual a 1 ent�o n�o pode
    for (int i=0;i<3&&erro==0;i++){
        Bloco aux = RodaHorarioZ(peca.bl[i]);
        if (peca.x+aux.x<0||peca.x+aux.x>=LARG||
            peca.y+aux.y<0||peca.y+aux.y>=ALT||
            peca.z+aux.z>=PROF || poco[peca.x+aux.x][peca.y+aux.y][peca.z+aux.z]!=0)

            erro = 1;
    }
    //Se a pe�a pode ser rotacionada, ent�o rotacionamos os blocos que n�o s�o pivores
    if (!erro){
        for (int i=0;i<3&&erro==0;i++){
            peca.bl[i] = RodaHorarioZ(peca.bl[i]);
        }
    }
}
//Tenta fazer com o que o valor de X dos blocos modifique
int TentaMoverX(Peca &peca,int dx){
    for (int i=0;i<4;i++){
        int novox = peca.x+peca.bl[i].x+dx;
        if (novox<0||novox>=LARG||poco[novox][peca.y+peca.bl[i].y][peca.z+peca.bl[i].z]!=0)
            return false;
    }
    peca.x+=dx;
    return true;
}
//Tenta fazer com o que o valor de Y dos blocos modifique
int TentaMoverY(Peca &peca,int dy){
    for (int i=0;i<4;i++){
        int novoy = peca.y+peca.bl[i].y+dy;
        if (novoy<0||novoy>=ALT||poco[peca.x+peca.bl[i].x][novoy][peca.z+peca.bl[i].z]!=0)
            return false;
    }
    peca.y+=dy;
    return true;
}
//Tenta fazer com o que o valor de Z dos blocos modifique (Cair)
int TentaMoverZ(Peca &peca,int dz){
    for (int i=0;i<4;i++){
        int novoz = peca.z+peca.bl[i].z+dz;
        if (novoz>=PROF||poco[peca.x+peca.bl[i].x][peca.y+peca.bl[i].y][novoz]!=0)
            return false;
    }
    peca.z+=dz;
    return true;
}
//Cria uma nova pe�a para o usu�rio mexer. A escolha ser� um valor aleat�rio entre 0 at� 3
Peca CriaPeca(int escolha){
    Peca peca;
    switch(escolha){
    case 0: //Pe�a S
        peca.x = 1;peca.y = 0;peca.z = 0;
        peca.bl[0].x = +1;peca.bl[0].y = 0;peca.bl[0].z = 0;
        peca.bl[1].x = 0;peca.bl[1].y = +1;peca.bl[1].z = 0;
        peca.bl[2].x = -1;peca.bl[2].y = +1;peca.bl[2].z = 0;
        peca.bl[3].x = peca.bl[3].y = peca.bl[3].z = 0;
        break;
    case 1: //Pe�a T
        peca.x = 1;peca.y = 0;peca.z = 0;
        peca.bl[0].x = +1;peca.bl[0].y = 0;peca.bl[0].z = 0;
        peca.bl[1].x = 0;peca.bl[1].y = +1;peca.bl[1].z = 0;
        peca.bl[2].x = -1;peca.bl[2].y = 0;peca.bl[2].z = 0;
        peca.bl[3].x = peca.bl[3].y = peca.bl[3].z = 0;
        break;
    case 2: //Pe�a L
        peca.x = 1;peca.y = 0;peca.z = 0;
        peca.bl[0].x = +1;peca.bl[0].y = 0;peca.bl[0].z = 0;
        peca.bl[1].x = -1;peca.bl[1].y = 0;peca.bl[1].z = 0;
        peca.bl[2].x = -1;peca.bl[2].y = +1;peca.bl[2].z = 0;
        peca.bl[3].x = peca.bl[3].y = peca.bl[3].z = 0;
        break;
    case 3: //Pe�a Quadrado
        peca.x = 1;peca.y = 0;peca.z = 0;
        peca.bl[0].x = +1;peca.bl[0].y = 0;peca.bl[0].z = 0;
        peca.bl[1].x = +1;peca.bl[1].y = +1;peca.bl[1].z = 0;
        peca.bl[2].x = 0;peca.bl[2].y = +1;peca.bl[2].z = 0;
        peca.bl[3].x = peca.bl[3].y = peca.bl[3].z = 0;
        break;
    case 4: //Pe�a I
        peca.x = 1;peca.y = 0;peca.z = 0;
        peca.bl[0].x = +1;peca.bl[0].y = 0;peca.bl[0].z = 0;
        peca.bl[1].x = -1;peca.bl[1].y = 0;peca.bl[1].z = 0;
        peca.bl[2].x = +2;peca.bl[2].y = 0;peca.bl[2].z = 0;
        peca.bl[3].x = peca.bl[3].y = peca.bl[3].z = 0;
        break;
    }
    return peca;
}
//Faz a pe�a cair
void TrataQueda(Peca &peca){
    if (TentaMoverZ(peca,+1)==false){ //Se a pe�a n�o puder cair mais, coloca os blocos no po�o
        for (int i=0;i<4;i++){
            poco[peca.x+peca.bl[i].x][peca.y+peca.bl[i].y][peca.z+peca.bl[i].z]=1;
        }
        for (int i=PROF-1;i>=0;i--){
            if (QtdBlocosNaProfundidade(i)==ALT*LARG){ //Se n�vel estiver completo, limpa o n�vel
                RemoveProfundidade(i);
                score += ALT*LARG*i;
                i--;
            }
        }
        if(peca.z!=0)
            peca = CriaPeca(rand() % 5); //Escolhe uma nova pe�a
        else
            endgame = 1;
    }
}
//Mostra qual placa de video do PC e vers�o do OpenGL
void ShowVersion(){
    const GLubyte* renderer = glGetString(GL_RENDERER); // Placa de Video
    const GLubyte* version = glGetString(GL_VERSION); // Vers�o do OpenGL
    printf("Renderer: %s\n", renderer);
    printf("OpenGL version supported %s\n", version);
}
void Setup2D(){
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, LARG_TELA, ALT_TELA, 0, -1, 1);
    glDisable(GL_DEPTH_TEST);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}
void Setup3D(){
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(70.0,1.0,0.1,500.0);
    glEnable(GL_DEPTH_TEST);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(0,0,-1);
}
int main( int argc, char* args[] ){
    PIG_Evento evento;
    PIG_Teclado meuTeclado = GetTeclado();;

    CriaJogo("BlockOut Upado");
    glewInit();
    GLuint idFrase=0;
    ShowVersion();
    srand(time(NULL));
    Peca peca = CriaPeca(rand()%5);
    enum numTelas{menu,jogo,sair};
    int mx,my,timerQueda = CriaTimer();

    while (JogoRodando()!=0 && endgame==0){
        evento = GetEvento();
        if(GetEstadoJogo()==menu){
            if(evento.tipoEvento==EVENTO_MOUSE&&evento.mouse.acao==MOUSE_PRESSIONADO){
                mx=evento.mouse.posX;
                my=evento.mouse.posY;
                if(mx>=0 && mx<=10 && my>=0 && my<=10)
                    SetEstadoJogo(jogo);
            }
            if (evento.tipoEvento==EVENTO_TECLADO&&evento.teclado.acao==TECLA_PRESSIONADA){
                if (evento.teclado.tecla==TECLA_ENTER){
                    SetEstadoJogo(jogo);
                }
            }
        }
        if(GetEstadoJogo()==jogo){
            if (evento.tipoEvento==EVENTO_TECLADO&&evento.teclado.acao==TECLA_PRESSIONADA){
                if (evento.teclado.tecla==TECLA_ESQUERDA){
                    TentaMoverX(peca,-1);
                }else if(evento.teclado.tecla==TECLA_DIREITA){
                    TentaMoverX(peca,+1);
                }else if(evento.teclado.tecla==TECLA_CIMA){
                    TentaMoverY(peca,+1);
                }else if(evento.teclado.tecla==TECLA_BAIXO){
                    TentaMoverY(peca,-1);
                }else if(evento.teclado.tecla==TECLA_z){
                    TentaRodarAntiHorarioZ(peca);
                }else if(evento.teclado.tecla==TECLA_x){
                    TentaRodarHorarioZ(peca);
                }else if(evento.teclado.tecla==TECLA_a){
                    TentaRodarAntiHorarioX(peca);
                }else if(evento.teclado.tecla==TECLA_s){
                    TentaRodarHorarioX(peca);
                }else if(evento.teclado.tecla==TECLA_q){
                    TentaRodarAntiHorarioY(peca);
                }else if(evento.teclado.tecla==TECLA_w){
                    TentaRodarHorarioY(peca);
                }else if(evento.teclado.tecla==TECLA_BARRAESPACO){
                    TrataQueda(peca);
                    ReiniciaTimer(timerQueda);
                }
            }
        }
        IniciaDesenho();

        if(GetEstadoJogo()==menu){
            Setup2D();
        }
        if(GetEstadoJogo()==jogo){
            if(endgame==0){
                Setup3D();
                if (TempoDecorrido(timerQueda)>0.9){
                    TrataQueda(peca);
                    ReiniciaTimer(timerQueda);
                }
                DesenhaPeca(peca);
                DesenhaPoco();
                DesenhaBlocos();
            }
        }

        EncerraDesenho();
    }
    FinalizaJogo();
    return 0;
}
