#include <fstream.h>
#include <iostream.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <stdlib.h>
#include <GL/glut.h>
#include <time.h>
#include "glui.h"
#include <stdarg.h>
#include <stdio.h>

#define MAXINGREDIENTS	200		//the maximum number of ingredients allowed
#define MAXDRINKS		1000	//the maximum number of drinks stored
#define MAXBUTTONS		100		//the maximum number of buttons on screen
#define MAXAMOUNTS		5		//the maximum number of different amounts of ingredients
#define MAXPIPES		8		//the maximum number of ingredients in pipes
#define DRINKSIZE		200		//the max drink size in ml
#define INTRO			1		//the introduction state
#define CHOICES			2		//the initial choices state - how to choose the cocktail
#define INGREDIENTS		3		//choosing by ingredients state
#define DRINKS			4		//view the drinks chosen with the ingredients
#define DRINKCHOSEN		5		//view the drink chosen through the ingredients
#define ALLDRINKS		6		//view all of the drinks
#define ALLDRINKCHOSEN	7		//view the drink chosen through the alphabetic list
#define MYOINGREDIENT	8		//make your own choose the ingredient
#define MYOAMOUNT		9		//make your own choose the amount
#define MYODRINK		10		//make your own view the drink so far
#define POSSIBLEINGREDIENTS			11		//order the current drink
#define LETTERS			12		//view drinks starting with a letter from drinks
#define ALLLETTERS		13		//view drinks starting with a letter from alldrinks
#define EDITINGREDIENT	14		//make your own edit ingredient
#define NEWINGREDIENT	15		//add a new ingredient
#define NEWDRINK		16		//add a new drink
#define SELECTAMOUNT	17		//select the amount to add
#define SELECTINGREDIENT	18	//select the ingredient to add
#define	SELECTNEWAMOUNT	19		//select a new amount to add
#define dynSize			2.3		//the size of the dynamic interface
#define xTrans			2.0		//the x transposition of the dynamic interface
#define yTrans			-0.3		//the y transposition of the dynamic interface

//define the structure for each drink
struct drink{
	char name[50];							// the name of the drink
	char ingredient[MAXINGREDIENTS][50];	// array of the ingredients
	int numIngredients;						// number of ingredients
	double amount[MAXINGREDIENTS];			// array of amounts of the ingredients
	char amountString[MAXINGREDIENTS][10];	// array of amounts of the ingredients (string)
	char textIngredients[5][60];			// ingredients in the drink to display
	int numlines;							// number of lines in the textIngredients
	char textIngredientsAmounts[MAXINGREDIENTS][60];	// ingredients and amounts in the drinks
	int validDrink;							// 1 if valid, 0 if not valid
	int visible;							// if the drink is visible
	int ballPosition;						// position in the movingButtons array
	int newBallPosition;					// position in the newMovingButtons array
	int drinkPos;							// position in the drinks array
};

// define the structure for each ball
struct ball {
	double posx;			// x position
	double posy;			// y position
	double posz;			// z position
	double size;			// size of the ball (radius)
	double red;			// amount of red of ball
	double green;		// amount of green of ball
	double blue;			// amount of blue of ball
	double textSize;			// size of the ball (radius)
	double textRed;			// amount of red of ball
	double textGreen;		// amount of green of ball
	double textBlue;			// amount of blue of ball
	char * text;	// the name of the drink
	char nextText[5][60];	// the ingredients of the drink
	int numlines;			// number of lines in the nextText
	int selected;		// if the ball is selected
	int drink;			// position in possible drinks array
};

// define the structure for each dynamic ball
struct dynamicball {
	double posx;			// x position
	double oldposx;		// last x position
	double posy;			// y position
	double oldposy;		// last y position
	double size;			// size of the ball (radius)
	double force;		// force on the ball
	double forcetheta;	// direction of the force
	double velx;			// x velocity
	double vely;			// y velocity
	double accx;			// x acceleration
	double accy;			// y acceleration
	double mass;			// mass of the ball
	int center;			// boolean determining if center ball ( 1 = centerball, 0 = not centerball)
	double red;			// amount of red of ball
	double green;		// amount of green of ball
	double blue;			// amount of blue of ball
	char * text;	// the name of the drink
	char nextText[5][60];	// the ingredients of the drink
	int numlines;			// number of lines in the nextText
	int drinkPos;		// the position of the drink in the drinks array
};

// define the structure for a letter
struct letter {
	int startDrink;
	int stopDrink;
	int numDrinks;
};

// define the structure for an ingredient
struct ingredient {
	int pipeNum;
	char name[50];
	double pipeAmount;
	char pipeAmountText[50];
};

char ingredients[MAXINGREDIENTS][50];			// the array of ingredients
struct ingredient pipeIngredients[MAXINGREDIENTS];	// the array of ingredients in pipes
struct ingredient newPipeIngredient;				// the new ingredient added
char newIngredient[50];							// the name of the new ingredient
int emptyPipe[MAXPIPES];						// if a pipe is empty, index is 0
int pipeToSet;									// the pipe index
int selected[MAXINGREDIENTS];					// whether the ingredient is selected
char selectedIngredients[MAXINGREDIENTS][50];	// the array of selected ingredients
struct drink drinks[MAXDRINKS];					// an array of drinks
int editedDrinkIndex;							// the drink index
struct drink possibleDrinks[MAXDRINKS];			// an array of possible drinks
struct ball buttons[MAXBUTTONS];				// an array of balls
struct drink MYODrink;							// the drink created through make your own
struct drink nullDrink;							// a null drink
struct drink lastDrink;							// the last drink that was ordered
struct drink newChosenDrink;					// the last drink that was chosen
struct dynamicball movingButtons[MAXDRINKS];	// an array of dynamic balls
struct dynamicball newMovingButtons[MAXDRINKS]; // an array of dynamic balls
struct letter alphabetInfo[26];					// an array of information for letters of the alphabet
double amount;				// amount of ingredients - used in init
char str[50];				// string used when reading in from files
char *str1, *stopstring;	// strings used when reading in from files
int numingredients;			// the number of ingredients
int numPipeIngredients;	// the number of possible ingredients
int chosenDrink;			// the number of the chosen drink
int set;					// the number of the set of the drinks
int ingredientsSet;			// the number of the set of the ingredients
int letterSet;				// the number of the set of the drinks for each letter
int MYOset;					// the number of the set for displaying the MYO ingredients
int letter;					// the index of the current letter
char buffer[255], *p;		// used when writing text to the display
int next,back,moreDrinks,lastDrinks,next2,next3,next4,back2; // used with buttons to check which button was pressed
int selectMiddleDrink = -1;	// button for order the middle drink
int editMiddleDrink = -1;	// button for edit the middle drink
int cancelMiddleDrink = -1;	// button for cancel the middle drink
int editIngredientIndex = -1;	// the index of the edited ingredient
int numDrinksDisplayed;		// the number of drinks displayed
int numIngredientsDisplayed;	// the number of ingredients displayed
int state = INTRO;			// the state of the interface, starts in introduction
int height = 450;			// height of the display
int width = 600;			// width of the display
int numButtons = 0;			// the number of buttons displayed
int numPossibleDrinks;		// the number of drinks in the possible drinks array
int numDrinks;				// the number of drinks total
int numLetters;				// the number of letters displayed
double mouseposx, mouseposy;	// x and y position of the mouse
int MYOindex;				// index refers to next position for an ingredient in the drink
int MYOAmounts [MAXAMOUNTS] = {10,15,30,60,90};	// the possible amounts - int
char MYOTextAmounts [MAXAMOUNTS][10] = {"10ml","15ml","30ml","60ml","90ml"}; // the possible amounts - string
char pipeNumbers[MAXPIPES][10] = {"1","2","3","4","5","6","7","8"};
char pipeAmounts[10][10] = {"1000ml","900ml","800ml","700ml","600ml","500ml","400ml","300ml","200ml","100ml"};
char pipeAmounts2[10][10] = {"950ml","850ml","750ml","650ml","550ml","450ml","350ml","250ml","150ml","50ml"};

//positions in the screen
double x1 = -4.5;			
double x2 = -3.0;
double x3 = -1.0;
double x4 = 0.0;
double x5 = 2.0;
double yinit = 3.5;
//dynamic balls variables
int numVisibleDrinks;		// the number of drinks displayed as balls
double otherAttractions[MAXDRINKS-1][MAXDRINKS-1];	// the array of the attractions of all of the visible drinks
double attrforce, attrforce1, attrforcex, attrforcey; // components of the attraction force
double dampforce;			// the damping force
double repforce, repforce1, repforcex, repforcey; // components of the repulsive force
double forcex, forcey;		// x and y components of the force on the ball
double attrtheta, attrtheta1, damptheta, reptheta, reptheta1;	// directions of the force components
double num;					// used when creating a new random number
double distance, velocity;	// used when determining the force on the ball
double percent;				// value of similarity placed in the attractions array
int centerballDrinks = 0;	// the center ball in the drinks array
int centerballBalls = 0;	// the center ball in the movingButtons array
int centerDrink = 0;		// the center baal in the buttons array
int showIngredients = -1;	// the drink to display the ingredients of
double k = 3;				// spring constant
double d = 2;				// damping constant
double e = 15;				// repulsion constant
double mass = 0.1;			// mass of the balls
double timestep = 0.001;		// timestep
int addAmountStart = 0;
int addAmountStop = 0;
int removeAmountStart = 0;
int removeAmountStop = 0;
int dynButton = 0;
int dynLastButton = 0;		// if a dynamic ball was last selected

int newDrink = 0;

int randomDrink = 0;

int removeStart = 0;

int dynamic = 0;

int drinkOrdered = 0;		// whether a drink has been ordered
int confirmed = 0;

char alphabet [27][5];

/*
 * void initIngredients (void)
 * 
 * reads in ingredients from a file (ingredients.txt)
 * stores these ingredients in an array
 * the closes the file
 * file is of the format:
 * ingredient 1
 * ...
 * final ingredient
 * end
 * 
 */
void initIngredients (void)
{
	int i,index;
	int a = 0;

	ifstream b_file("ingredients.txt");
	b_file.getline(str,50,'\n');
	while(strcmp(str,"end") != 0){
		if (a == 0){
			strcpy( ingredients[a], str );
		} else {
			index = 0;
			for (i=0; i<a; i++){
				if (_stricmp(str,ingredients[i]) == 1){
					index = i+1;
				}
			}
			for (i=a; i>index; i--){
				strcpy(ingredients[i],ingredients[i-1]);
			}
			strcpy( ingredients[index], str );
		}

		b_file.getline(str,50,'\n');
		a++;
	}
	strcpy(ingredients[a],"null");
	numingredients = a;
	b_file.close();    
}

int countNumIngredients(void){
	int i;
	i=0;
	while (strcmp(ingredients[i],"null") != 0){
		i++;
	}
	return i;
}


void writeIngredients(void){

	int j,len;
	ofstream b_file("ingredients.txt");
	
	for (j=0; j<numingredients; j++){
		len = strlen(ingredients[j]);
		b_file.write(ingredients[j],len );
		b_file.write("\n", 1);
	}
	b_file.write("end",3);
	b_file.close();
}


void initPipeAmounts (void)
{
	int a = 0;
	int decimal, sign;

	ifstream b_file("pipeingredients.txt");
	b_file.getline(str,50,'\n');
	while(strcmp(str,"end") != 0){
		strcpy( pipeIngredients[a].name, str );
		b_file.getline(str,50,'\n');
		pipeIngredients[a].pipeAmount = (double)strtod(str,&stopstring);
		strcpy(pipeIngredients[a].pipeAmountText,_fcvt((pipeIngredients[a].pipeAmount),0,&decimal,&sign));
		b_file.getline(str,50,'\n');
		pipeIngredients[a].pipeNum = (int)strtod(str,&stopstring);
		b_file.getline(str,50,'\n');
		if (pipeIngredients[a].pipeNum <= MAXPIPES){
			a++;
		} else {
			strcpy(pipeIngredients[a].name,"");
			pipeIngredients[a].pipeAmount = 0;
			pipeIngredients[a].pipeNum = 0;
		}
	}
	if (a != MAXPIPES){
		strcpy(pipeIngredients[a].name,"null");
	}
	numPipeIngredients = a;
	b_file.close();    
}

int countNumPipeIngredients(void){
	int i;
	i=0;
	while ((strcmp(pipeIngredients[i].name,"null")!=0) && (i<MAXPIPES)){
		i++;
	}
	return i;
}

void writePipeAmounts(void){

	int j,len;
	const char *t;
	int decimal, sign;
	ofstream b_file("pipeingredients.txt");
	
	for (j=0; j<numPipeIngredients; j++){
		len = strlen(pipeIngredients[j].name);
		b_file.write(pipeIngredients[j].name,len );
		b_file.write("\n", 1);
		t = _fcvt((pipeIngredients[j].pipeAmount),0,&decimal,&sign);
		len = strcspn(t," ");
		b_file.write(t,len);
		b_file.write("\n", 1);
		t = _fcvt((pipeIngredients[j].pipeNum),0,&decimal,&sign);
		len = strcspn(t," ");
		b_file.write(t,len);
		b_file.write("\n", 1);
	}
	b_file.write("end",4);
	b_file.close();

	ofstream c_file("ingredients2.txt");
	for (j=0; j<numPipeIngredients; j++){
		len = strlen(pipeIngredients[j].name);
		c_file.write(pipeIngredients[j].name,len );
		c_file.write("\n", 1);
	}
	c_file.write("end",4);
	c_file.close();


}

/*
 * void initDrinks (void)
 * 
 * reads in drinks from a file (drinks.txt)
 * stores these drinks in an array, with the ingredients and amounts
 * then closes the file
 * file is of the format:
 * name of drink
 * ingredient: amount
 * ...
 * final ingredient: amount
 * end
 * ...
 * last
 * drinks are in alphabetical order (?)
 */
void initDrinks(void)
{
	int g,a,b,i,count,index,decimal,sign;

	ifstream a_file("drinks.txt"); 
	a_file.getline(str,50,'\n');
	g = 0;
	index = 0;
	while(strcmp(str,"last") != 0){
		if (g == 0){
			strcpy( drinks[g].name, str );
		} else {
			index = 0;
			for (i=0; i<g; i++){
				if (_stricmp(str,drinks[i].name) == 1){
					index = i+1;
				}
			}
			for (i=g; i>index; i--){
				drinks[i] = drinks[i-1];
			}
			strcpy( drinks[index].name, str );
		}
		drinks[index].validDrink = 1;
		for (b=0;b<5;b++){
			strcpy(drinks[index].textIngredients[b],"");
		}
		a_file.getline(str,50,'\n');
		a = 0;
		b = 0;
		while (strcmp(str,"end") != 0){
			str1 = strtok( str, ":" );
			strcpy(drinks[index].ingredient[a],str1);
			if (strlen(drinks[index].textIngredients[b]) > 22){
				b++;
			}
			strcat(drinks[index].textIngredients[b],drinks[index].ingredient[a]);
			strcat(drinks[index].textIngredients[b],", ");
			strcpy(drinks[index].textIngredientsAmounts[a],drinks[index].ingredient[a]);
			strcat(drinks[index].textIngredientsAmounts[a]," ");
			count = 0;
			for (i = 0; i<numingredients; i++){
				if (strcmp(drinks[index].ingredient[a], ingredients[i]) == 0)
						count = count++;
			}
			if (count != 1)
				drinks[index].validDrink = 0;

			str1 = strtok( NULL, " " );
			amount = (double)strtod(str1,&stopstring);
			drinks[index].amount[a] = amount;
			strcpy(drinks[index].amountString[a],(_fcvt(amount,0,&decimal,&sign)));
			strcat(drinks[index].textIngredientsAmounts[a],(_fcvt(amount,0,&decimal,&sign)));
			strcat(drinks[index].textIngredientsAmounts[a], "ml");
			a++;
			a_file.getline(str,50,'\n');
		}
		drinks[index].numlines = b;
		strcpy(drinks[index].ingredient[a],"null");
		a_file.getline(str,50,'\n');
		drinks[index].numIngredients = a;
		g++;
	}
	strcpy(drinks[g].name, "null");
	a_file.close();    
	numDrinks = g;
}

void writeDrinks(void){

	int i,j,len;
	ofstream b_file("drinks.txt");
	
	for (j=0; j<numDrinks; j++){
		len = strlen(drinks[j].name);
		b_file.write(drinks[j].name,len );
		b_file.write("\n", 1);
		for (i=0; i<drinks[j].numIngredients; i++){
			len = strlen(drinks[j].ingredient[i]);
			b_file.write(drinks[j].ingredient[i],len);
			b_file.write(": ",2);
			len = strlen(drinks[j].amountString[i]);
			b_file.write(drinks[j].amountString[i],len);
			b_file.write(".0\n", 3);
		}
		b_file.write("end\n",4);
	}
	b_file.write("last\n",5);
	b_file.close();
}

/*
 * void init(void)
 * 
 * initialisation fuction
 * clears the color, and sets up constants
 */
void init(void)
{
   glClearColor(0.0, 0.0, 0.0, 0.0);
   glShadeModel (GL_FLAT);

   if (dynamic == 1){
	   x1 = -4.5;
	   x2 = -3.0;
	   x3 = -2.0;
	   x4 = 0.0;
	   x5 = 2.0;
	   yinit = 3.5;
   } else {
   	   x1 = -3.0;
	   x2 = 2.0;
	   x3 = -1.0;
	   x4 = 0.0;
	   x5 = 2.0;
	   yinit = 3.5;
   }


	strcpy(alphabet[0],"A");
	strcpy(alphabet[1],"B");
	strcpy(alphabet[2],"C");
	strcpy(alphabet[3],"D");
	strcpy(alphabet[4],"E");
	strcpy(alphabet[5],"F");
	strcpy(alphabet[6],"G");
	strcpy(alphabet[7],"H");
	strcpy(alphabet[8],"I");
	strcpy(alphabet[9],"J");
	strcpy(alphabet[10],"K");
	strcpy(alphabet[11],"L");
	strcpy(alphabet[12],"M");
	strcpy(alphabet[13],"N");
	strcpy(alphabet[14],"O");
	strcpy(alphabet[15],"P");
	strcpy(alphabet[16],"Q");
	strcpy(alphabet[17],"R");
	strcpy(alphabet[18],"S");
	strcpy(alphabet[19],"T");
	strcpy(alphabet[20],"U");
	strcpy(alphabet[21],"V");
	strcpy(alphabet[22],"W");
	strcpy(alphabet[23],"X");
	strcpy(alphabet[24],"Y");
	strcpy(alphabet[25],"Z");
	strcpy(alphabet[26]," ");
}


/*
 * void makeButton(int position,double posx,double posy,double posz,
 *				double size,double red,double green,double blue,
 *				double textSize,	double textRed,double textGreen,
 *				double textBlue,char * text,char nextText[5][60],
 *				int selected,int drink)
 *
 * makes a button in the position indicated in the buttons array
 * with the values given
 */
void makeButton(int position,double posx,double posy,double posz,
				double size,double red,double green,double blue,
				double textSize,	double textRed,double textGreen,
				double textBlue,char * text,char nextText[5][60],
				int selected,int drink)
{
	int b;

	buttons[position].posx = posx;
	buttons[position].posy = posy;
	buttons[position].posz = posz;
	buttons[position].size = size;
	buttons[position].red = red;
	buttons[position].green = green;
	buttons[position].blue = blue;
	buttons[position].textSize = textSize;
	buttons[position].textRed = textRed;
	buttons[position].textGreen = textGreen;
	buttons[position].textBlue = textBlue;
	buttons[position].text = text;
	for (b=0; b<5; b++){
		strcpy(buttons[position].nextText[b], nextText[b]);
	}
	buttons[position].selected = selected;
	buttons[position].drink = drink;
}


/*
 * void initMYODrink (void)
 *
 * initialises the MYODrink structure
 */
void initMYODrink (void)
{
	int a;

	for (a=0; a<MAXINGREDIENTS; a++){
		MYODrink.amount[a] = 0.0;
		strcpy(MYODrink.amountString[a],"");
		strcpy(MYODrink.ingredient[a],"");
		strcpy(MYODrink.textIngredientsAmounts[a],"");
	}
	strcpy(MYODrink.name,"Make your own");
	MYODrink.numIngredients = 0;
	for (a=0; a<5; a++)
		strcpy(MYODrink.textIngredients[a],"");
	MYODrink.validDrink = 1;
	MYOindex = 0;
}


/*
 * void initBalls (void)
 *
 * initialises the stationary balls ie the buttons
 */				
void initBalls (void)
{
	int a;
	char nT[5][60];	// the ingredients of the drink
	double x = 0.0;		// x position		
	double y = 0.0;		// y position
	double z = 0.0;		// z position
	double s = 0.0;		// size of the ball (radius)
	double r = 0.0;		// amount of red of ball
	double g = 0.0;		// amount of green of ball
	double b = 0.0;		// amount of blue of ball
	double tS = 0.0;		// size of the ball (radius)
	double tR = 0.0;		// amount of red of ball
	double tG = 0.0;		// amount of green of ball
	double tB = 0.0;		// amount of blue of ball
	char * t = "";		// the name of the drink
	int sel = 0;		// if the ball is selected
	int d = 0;			// position in possible drinks array
	int j=0;

	for (a=0; a<5; a++){
			strcpy(nT[a], "");
	}
	for (j = 0; j < MAXBUTTONS; j++){
		makeButton(j,x,y,z,s,r,g,b,tS,tR,tG,tB,t,nT,sel,d);
	}
}


/*
 * void initDynamicBalls(void)
 * initialises the array of possible balls
 * places them all at the center with no velocity
 * also sets the colour of the ball
 */
void initDynamicBalls(void)
{
	int j;
	
	for (j = 0; j < MAXDRINKS; j++){
		movingButtons[j].posx = 0.0;
		movingButtons[j].posy = 0.0;
		movingButtons[j].velx = 0.0;
		movingButtons[j].vely = 0.0;
		movingButtons[j].accx = 0.0;
		movingButtons[j].accy = 0.0;
		movingButtons[j].size = 0.2;
		movingButtons[j].mass = 0.1;
		movingButtons[j].center = 0;
		movingButtons[j].green = 0.0;
		movingButtons[j].blue = 0.0;
		movingButtons[j].red = 0.0;
	}
}


/*
 * void initNewDynamicBalls(void)
 * initialises the array of possible balls
 * places them all at the center with no velocity
 * also sets the colour of the ball
 */
void initNewDynamicBalls(void)
{
	int j;
	
	for (j = 0; j < MAXDRINKS; j++){
		newMovingButtons[j].posx = 0.0;
		newMovingButtons[j].posy = 0.0;
		newMovingButtons[j].velx = 0.0;
		newMovingButtons[j].vely = 0.0;
		newMovingButtons[j].accx = 0.0;
		newMovingButtons[j].accy = 0.0;
		newMovingButtons[j].size = 0.2;
		newMovingButtons[j].mass = 0.1;
		newMovingButtons[j].center = 0;
		newMovingButtons[j].green = 0.0;
		newMovingButtons[j].blue = 0.0;
		newMovingButtons[j].red = 0.0;
	}
}


void clearSelected (){
	for (int i = 0; i< MAXINGREDIENTS; i++){
		selected[i] = 0;
	}
}


/*
 * void openingBalls (void)
 * 
 * the opening screen for the interface
 */
void openingBalls (void)
{
	int a;
	char nT[5][60];	// the ingredients of the drink
	double x = 0.0;		// x position		
	double y = 0.0;		// y position
	double z = 0.0;		// z position
	double s = 0.0;		// size of the ball (radius)
	double r = 0.0;		// amount of red of ball
	double g = 0.0;		// amount of green of ball
	double b = 0.0;		// amount of blue of ball
	double tS = 0.0;		// size of the ball (radius)
	double tR = 0.0;		// amount of red of ball
	double tG = 0.0;		// amount of green of ball
	double tB = 0.0;		// amount of blue of ball
	char * t = "";		// the name of the drink
	int sel = 0;		// if the ball is selected
	int d = 0;			// position in possible drinks array
	int j=0;

	for (a=0; a<5; a++){
			strcpy(nT[a], "");
	}
	initBalls();
	x = x4;
	s = 0.7;
	r = 1.0;
	makeButton(j,x,y,z,s,r,g,b,tS,tR,tG,tB,t,nT,sel,d);
	next = j;
	j++;
	s = 0.0;
	r = 0.0;
	x = x4-0.27;
	t = "enter";
	tS = 1.0;
	makeButton(j,x,y,z,s,r,g,b,tS,tR,tG,tB,t,nT,sel,d);
	j++;
	x = x4-1.2;
	y = 2.0;
	tR = 1.0;
	tG = 1.0;
	tB = 1.0;
	t = "PipeDream";
	tS = 2.0;
	makeButton(j,x,y,z,s,r,g,b,tS,tR,tG,tB,t,nT,sel,d);
	j++;
	numButtons = j;
}


/*
 * void choicesBalls(void)
 *
 * the initial choices of how to choose the drink
 * ie select ingredients, view all, make your own
 */
void choicesBalls(void)
{
	int a;
	char nT[5][60];	// the ingredients of the drink
	double x = 0.0;		// x position		
	double y = 0.0;		// y position
	double z = 0.0;		// z position
	double s = 0.0;		// size of the ball (radius)
	double r = 0.0;		// amount of red of ball
	double g = 0.0;		// amount of green of ball
	double b = 0.0;		// amount of blue of ball
	double tS = 0.0;		// size of the ball (radius)
	double tR = 0.0;		// amount of red of ball
	double tG = 0.0;		// amount of green of ball
	double tB = 0.0;		// amount of blue of ball
	char * t = "";		// the name of the drink
	int sel = 0;		// if the ball is selected
	int d = 0;			// position in possible drinks array
	int j=0;

	for (a=0; a<5; a++){
			strcpy(nT[a], "");
	}
	initBalls();
	s = 0.3;
	r = 0.5;
	b = 0.5;
	t = "Drinks Database";
	x = x1;
	y = yinit - (j+2);
	tR = 1.0;
	tG = 1.0;
	tB = 1.0;
	tS = 1.0;
	makeButton(j,x,y,z,s,r,g,b,tS,tR,tG,tB,t,nT,sel,d);
	next2 = j;
	j++;
	y = yinit - (j+2);
	t = "Ingredients in PipeDream";
	makeButton(j,x,y,z,s,r,g,b,tS,tR,tG,tB,t,nT,sel,d);
	next = j;
	j++;
	y = yinit - (j+2);
	t = "Ingredients Database";
	makeButton(j,x,y,z,s,r,g,b,tS,tR,tG,tB,t,nT,sel,d);
	next3 = j;
	j++;
	t = "Back to Start";
	b = 0.5;
	r = 0.0;
	x = x1;
	y = yinit-7.3;
	s = 0.3;
	tS = 1.0;
	makeButton(j,x,y,z,s,r,g,b,tS,tR,tG,tB,t,nT,sel,d);
	back = j;
	j++;

	numButtons = j;
}


/*
 * void displayBall(double xpos, double ypos, double zpos, double red, 
 *				 double green, double blue, double size)
 *
 * displays a ball with the given information
 */
void displayBall(double xpos, double ypos, double zpos, double red, 
				 double green, double blue, double size)
{
   glPushMatrix();
   // translate the origin
   glTranslated (xpos, ypos, zpos);
   // set the colour
   glColor3f(red, green, blue);
   // draw a solid sphere of the size of the ball
   glutSolidSphere(size, 32, 32);
   glPopMatrix();
}

/*
 * void displayText(double xpos, double ypos, double zpos, double red,
 *				 double green, double blue, char * text, double size)
 *
 * displays text with the given information
 */
void displayText(double xpos, double ypos, double zpos, double red, 
				 double green, double blue, char * text, double size)
{
	va_list args;

	glPushMatrix();
	// translate the origin
	glTranslated (xpos, ypos, zpos);
	va_start(args, text);
	vsprintf(buffer, text, args);
	va_end(args);
	glPushAttrib(GL_ENABLE_BIT);
	// set the colour
	glColor3f(red,green,blue);
	glScalef(0.0014*size,0.0016*size,0.0016*size);
	for(p = buffer; *p; p++)
		glutStrokeCharacter(GLUT_STROKE_MONO_ROMAN, *p);
	glPopAttrib();
	glPopMatrix();
}

/*
 * void drawLine (double xpos1, double ypos1, double xpos2, double ypos2, 
 *			   double red, double green, double blue)
 *
 * draws a line with the given information
 */
void drawLine (double xpos1, double ypos1, double xpos2, double ypos2, 
			   double red, double green, double blue)
{
		glPushMatrix();
		glColor3f(red,green,blue);
		glBegin (GL_LINES);
		glVertex2f (xpos1, ypos1);
		glVertex2f (xpos2, ypos2);
		glEnd ();
		glPopMatrix();
}

void drawDrinkPicture(struct drink drinkToDraw){
	double y;
	int colourSet;
	double drinkHeight = 4.0;
	double drinkWidth = 0.5;
   
	colourSet = 0;
	glPushMatrix();
	// translate the origin
	glTranslated (x1-1.2, yinit - 5.0, 0.0);
	// set the colour
	drawLine(0.0,0.0,0.0,drinkHeight,1.0,1.0,1.0);
	drawLine(drinkWidth,drinkHeight,drinkWidth,0.0,1.0,1.0,1.0);
	drawLine(drinkWidth,0.0,0.0,0.0,1.0,1.0,1.0);
	glColor3f(1.0, 1.0, 1.0);
	for (int i=0; i<drinkToDraw.numIngredients; i++){
		y= drinkHeight * (drinkToDraw.amount[i] / DRINKSIZE);
		glColor3f(1.0 - colourSet*0.2, 0.0, colourSet*0.2);
		glRectf(0.0,0.0,drinkWidth,y);
		glTranslated(0.0,y,0.0);
		if (colourSet != 5){
			colourSet = colourSet + 1;
		} else {
			colourSet = 0;
		}
	}
	glPopMatrix();
}


/*
 * void display(void)
 *
 * the display function - displays the stationary balls and text
 */
void display(void)
{
	int i,b;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if ((state == MYODRINK) || (state == MYOAMOUNT) || (state == MYOINGREDIENT)){
		drawDrinkPicture(MYODrink);
	}
	if (state == EDITINGREDIENT){
		drawDrinkPicture(MYODrink);	
	}


	// draw the stationary buttons
	for (i=0; i<numButtons; i++){
		displayBall(buttons[i].posx, buttons[i].posy, buttons[i].posz,
			buttons[i].red, buttons[i].green, buttons[i].blue, buttons[i].size);
		displayText(buttons[i].posx + buttons[i].size, buttons[i].posy, buttons[i].posz,
			buttons[i].textRed, buttons[i].textGreen, buttons[i].textBlue,
			buttons[i].text, buttons[i].textSize);
		for(b=0;b<5;b++){
			displayText(buttons[i].posx + buttons[i].size, buttons[i].posy - (1+b)*(buttons[i].textSize/5.5), 
				buttons[i].posz, buttons[i].textRed, (buttons[i].textGreen-1.0), buttons[i].textBlue,
				buttons[i].nextText[b], (buttons[i].textSize*0.8));
		}
	}
	glutSwapBuffers();
}

void drawIngredients(int ingrSet){
	int i,j;
	int a;
	int decimal,sign;
//	int start,remaining;
	char nT[5][60];	// the ingredients of the drink
	double x = 0.0;		// x position		
	double y = 0.0;		// y position
	double z = 0.0;		// z position
	double s = 0.0;		// size of the ball (radius)
	double r = 0.0;		// amount of red of ball
	double g = 0.0;		// amount of green of ball
	double b = 0.0;		// amount of blue of ball
	double tS = 0.0;		// size of the ball (radius)
	double tR = 0.0;		// amount of red of ball
	double tG = 0.0;		// amount of green of ball
	double tB = 0.0;		// amount of blue of ball
	char * t = "";		// the name of the drink
	int sel = 0;		// if the ball is selected
	int d = 0;			// position in possible drinks array

	for (a=0; a<5; a++){
			strcpy(nT[a], "");
	}
	initBalls();

	s = 0.0;
	tR = 1.0;
	tB = 1.0;
	tG = 1.0;
	tS = 0.8;
	r = 1.0;
	x = x1 - 0.5;
	for (i=0; i<MAXPIPES; i++){
		emptyPipe[i] = 0;
		for (j=0; j<numPipeIngredients; j++){
			if (pipeIngredients[j].pipeNum == (i+1)){
				t = pipeIngredients[j].name;
				sel = 0;
				y = yinit - (i+2)*0.5;
				makeButton(i,x,y,z,s,r,g,b,tS,tR,tG,tB,t,nT,sel,d);
				emptyPipe[i] = 1;
			}
		}
	}
	numIngredientsDisplayed = MAXPIPES;

	tR = 1.0;
	tB = 1.0;
	tG = 1.0;
	tS = 0.8;
	r = 0.5;
	b = 0.5;
	s = 0.2;
	x = x1 + 2.0;
	for (i=MAXPIPES; i<(2*MAXPIPES); i++){
		for (j=0; j<numPipeIngredients; j++){
			if (pipeIngredients[j].pipeNum == ((i-MAXPIPES)+1)){
				t = pipeIngredients[j].pipeAmountText;
				y = yinit - ((i-MAXPIPES)+2)*0.5;
				makeButton(i,x,y,z,s,r,g,b,tS,tR,tG,tB,t,nT,sel,d);
			}
		}
	}

	s = 0.0;
	x = x1 - 1.5;
	for (i=(2*MAXPIPES); i<(3*MAXPIPES); i++){
		t = pipeNumbers[(i-2*MAXPIPES)];
		sel = 0;
		y = yinit - ((i-2*MAXPIPES)+2)*0.5;
		makeButton(i,x,y,z,s,r,g,b,tS,tR,tG,tB,t,nT,sel,d);
	}


	x = x2;
	s = 0.2;
	for (i=(3*MAXPIPES); i<(4*MAXPIPES); i++){
		y = yinit - ((i-3*MAXPIPES)+2)*0.5;
		if (emptyPipe[i-3*MAXPIPES] == 0){
			t = "Add ingredient";
		} else {
			t = "Remove ingredient";
		}
		makeButton(i,x,y,z,s,r,g,b,tS,tR,tG,tB,t,nT,sel,d);
	}

	t = "Pipe number";
	r = 0.0;
	x = x1 - 2.0;
	y = yinit-0.5;
	s = 0.0;
	tS = 0.8;
	makeButton(i,x,y,z,s,r,g,b,tS,tR,tG,tB,t,nT,sel,d);
	i++;
	t = "Ingredient";
	x = x1 - 0.5;
	y = yinit-0.5;
	makeButton(i,x,y,z,s,r,g,b,tS,tR,tG,tB,t,nT,sel,d);
	i++;
	t = "Edit Amount in Pipe";
	x = x1 + 2.0;
	y = yinit-0.5;
	makeButton(i,x,y,z,s,r,g,b,tS,tR,tG,tB,t,nT,sel,d);
	i++;

	t = "Ingredients in PipeDream";
	r = 0.0;
	x = x1;
	y = yinit+0.5;
	s = 0.0;
	tS = 1.5;
	makeButton(i,x,y,z,s,r,g,b,tS,tR,tG,tB,t,nT,sel,d);
	i++;

	if (numPossibleDrinks != 0){
		t = _fcvt(numPossibleDrinks,0,&decimal,&sign);
	} else {
		t = "0";
	}
	x = x2 + 1.0;
	y = yinit-6.5;
	s = 0.0;
	tS = 0.8;
	makeButton(i,x,y,z,s,r,g,b,tS,tR,tG,tB,t,nT,sel,d);
	i++;
	t = "Number of drinks possible:";
	x = x2-1.0;
	y = yinit-6.0;
	makeButton(i,x,y,z,s,r,g,b,tS,tR,tG,tB,t,nT,sel,d);
	i++;


	t = "View Drinks";
	b = 0.5;
	r = 0.5;
	x = x1;
	y = yinit - (1+12)*0.5;
	s = 0.3;
	tR = 1.0;
	tB = 1.0;
	tG = 1.0;
	tS = 1.0;
	makeButton(i,x,y,z,s,r,g,b,tS,tR,tG,tB,t,nT,sel,d);
	next = i;
	i++;
	t = "with these";
	b = 0.0;
	r = 0.0;
	x = x1 + 0.5;
	y = yinit - (1+12)*0.5 - 0.2;
	s = 0.0;
	tR = 1.0;
	tB = 1.0;
	tG = 1.0;
	tS = 0.8;
	makeButton(i,x,y,z,s,r,g,b,tS,tR,tG,tB,t,nT,sel,d);
	i++;
	t = " ingredients";
	b = 0.0;
	r = 0.0;
	x = x1 + 0.5;
	y = yinit - (1+12)*0.5 - 0.4;
	s = 0.0;
	tR = 1.0;
	tB = 1.0;
	tG = 1.0;
	tS = 0.8;
	makeButton(i,x,y,z,s,r,g,b,tS,tR,tG,tB,t,nT,sel,d);
	i++;
	t = "Main menu";
	b = 0.5;
	x = x1;
	y = yinit-7.3;
	s = 0.3;
	tS = 1.0;
	makeButton(i,x,y,z,s,r,g,b,tS,tR,tG,tB,t,nT,sel,d);
	back = i;
	i++;
	
	numButtons = i;
}

void drawPossibleIngredients(int drawRemove){
	int i,j;
	int a;
	int start,remaining;
	char nT[5][60];	// the ingredients of the drink
	double x = 0.0;		// x position		
	double y = 0.0;		// y position
	double z = 0.0;		// z position
	double s = 0.0;		// size of the ball (radius)
	double r = 0.0;		// amount of red of ball
	double g = 0.0;		// amount of green of ball
	double b = 0.0;		// amount of blue of ball
	double tS = 0.0;		// size of the ball (radius)
	double tR = 0.0;		// amount of red of ball
	double tG = 0.0;		// amount of green of ball
	double tB = 0.0;		// amount of blue of ball
	char * t = "";		// the name of the drink
	int sel = 0;		// if the ball is selected
	int d = 0;			// position in possible drinks array

	for (a=0; a<5; a++){
			strcpy(nT[a], "");
	}
	initBalls();
	start = 20*ingredientsSet;
	remaining = numingredients - start;
	if (remaining < 20){
		moreDrinks = -1;
		numIngredientsDisplayed = remaining;
		for (i=0; i<remaining;i++){
			t = ingredients[i+start];
			r = 0.5;
			b = 0.5;
			if (i<remaining/2){
				x = x1;
				y = yinit - (i+2)*0.5;
			} else {
				if (dynamic == 1){
				x = x2+1.0;
				} else {
				x = x2;
				}
				y = yinit - ((i+2)-(remaining/2))*0.5;
			}
			s = 0.2;
			if (selected[i+start] == 1){
				r = 0.0;
				b = 1.0;
				sel = 1;
			} else {
				r = 1.0;
				b = 0.0;
				sel = 0;
			}
			tR = 1.0;
			tB = 1.0;
			tG = 1.0;
			tS = 0.8;
			makeButton(i,x,y,z,s,r,g,b,tS,tR,tG,tB,t,nT,sel,d);
		}
	} else {
		numIngredientsDisplayed = 20;
		for (i=0; i<20;i++){
			t = ingredients[i+start];
			r = 0.5;
			b = 0.5;
			if (i<10){
				x = x1;
				y = yinit - (i+2)*0.5;
			} else {
				if (dynamic == 1){
				x = x2+1.0;
				} else {
				x = x2;
				}
				y = yinit - ((i+2)-(10))*0.5;
			}
			s = 0.2;
			if (selected[i+start] == 1){
				r = 0.0;
				b = 1.0;
				sel = 1;
			} else {
				r = 1.0;
				b = 0.0;
				sel = 0;
			}
			tR = 1.0;
			tB = 1.0;
			tG = 1.0;
			tS = 0.8;
			makeButton(i,x,y,z,s,r,g,b,tS,tR,tG,tB,t,nT,sel,d);
		}
		
		y = yinit;
		x = x1;
		s = 0.2;
		r = 0.5;
		b = 1.0;
		tS = 1.0;
		t = "more ingredients";
		for (a=0;a<5;a++){
			strcpy(nT[a], "");
		}
		d = 0;
		makeButton(i,x,y,z,s,r,g,b,tS,tR,tG,tB,t,nT,sel,d);
		moreDrinks = i;
	}
	if (ingredientsSet != 0){
		i++;
		x = x1;
		y = yinit - 0.5;
		s = 0.2;
		r = 0.5;
		g = 0.0;
		b = 1.0;
		tS = 1.0;
		t = "previous ingredients";
		for (a=0;a<5;a++){
			strcpy(nT[a], "");
		}
		d = 0;
		makeButton(i,x,y,z,s,r,g,b,tS,tR,tG,tB,t,nT,sel,d);
		lastDrinks = i;
	} else {
		lastDrinks = -1;
	}
	i++;

	removeStart = i;
	if (drawRemove){
		for (j=i; j<i+numIngredientsDisplayed;j++){
			t = "remove";
			r = 0.0;
			b = 0.8;
			if (j-i<numIngredientsDisplayed/2){
				x = x1 + 3.0;
				y = yinit - (j-i+2)*0.5;
			} else {
				x = x2 + 3.0;
				y = yinit - ((j-i+2)-(numIngredientsDisplayed/2))*0.5;
			}
			s = 0.15;
			tR = 1.0;
			tB = 1.0;
			tG = 1.0;
			tS = 0.6;
			makeButton(j,x,y,z,s,r,g,b,tS,tR,tG,tB,t,nT,sel,d);
		}
		i = j;
	}

	t = "Main menu";
	b = 0.5;
	x = x1;
	y = yinit-7.3;
	s = 0.3;
	tS = 1.0;
	makeButton(i,x,y,z,s,r,g,b,tS,tR,tG,tB,t,nT,sel,d);
	back = i;
	i++;


	t = "Ingredients";
	r = 0.0;
	x = x1;
	y = yinit+0.5;
	s = 0.0;
	tS = 1.5;
	makeButton(i,x,y,z,s,r,g,b,tS,tR,tG,tB,t,nT,sel,d);
	i++;
	t = "View Drinks";
	b = 0.5;
	r = 0.5;
	x = x1;
	y = yinit - (1+12)*0.5;
	s = 0.3;
	tR = 1.0;
	tB = 1.0;
	tG = 1.0;
	tS = 1.0;
	makeButton(i,x,y,z,s,r,g,b,tS,tR,tG,tB,t,nT,sel,d);
	next = i;
	i++;
	t = "with selected";
	b = 0.0;
	r = 0.0;
	x = x1 + 0.5;
	y = yinit - (1+12)*0.5 - 0.2;
	s = 0.0;
	tR = 1.0;
	tB = 1.0;
	tG = 1.0;
	tS = 0.8;
	makeButton(i,x,y,z,s,r,g,b,tS,tR,tG,tB,t,nT,sel,d);
	i++;
	t = " ingredients";
	b = 0.0;
	r = 0.0;
	x = x1 + 0.5;
	y = yinit - (1+12)*0.5 - 0.4;
	s = 0.0;
	tR = 1.0;
	tB = 1.0;
	tG = 1.0;
	tS = 0.8;
	makeButton(i,x,y,z,s,r,g,b,tS,tR,tG,tB,t,nT,sel,d);
	i++;
	t = "Add new ingredient";
	b = 0.5;
	r = 0.5;
	x = x2;
	y = yinit - (1+12)*0.5;
	s = 0.3;
	tR = 1.0;
	tB = 1.0;
	tG = 1.0;
	tS = 1.0;
	makeButton(i,x,y,z,s,r,g,b,tS,tR,tG,tB,t,nT,sel,d);
	next2 = i;
	i++;

	
	numButtons = i;
}



/*
 * void drawMYOIngredients(void)
 *
 * make your own option - displays ingredients
 */
void drawMYOIngredients(int ingrSet)
{
	int i;
	int a;
	int start, remaining;
	char nT[5][60];	// the ingredients of the drink
	double x = 0.0;		// x position		
	double y = 0.0;		// y position
	double z = 0.0;		// z position
	double s = 0.0;		// size of the ball (radius)
	double r = 0.0;		// amount of red of ball
	double g = 0.0;		// amount of green of ball
	double b = 0.0;		// amount of blue of ball
	double tS = 0.0;		// size of the ball (radius)
	double tR = 0.0;		// amount of red of ball
	double tG = 0.0;		// amount of green of ball
	double tB = 0.0;		// amount of blue of ball
	char * t = "";		// the name of the drink
	int sel = 0;		// if the ball is selected
	int d = 0;			// position in possible drinks array

	for (a=0; a<5; a++){
			strcpy(nT[a], "");
	}
	initBalls();
	start = 20*ingrSet;
	remaining = numingredients - start;
	if (remaining < 20){
		numIngredientsDisplayed = remaining;
		for (i=0; i<remaining;i++){
			t = ingredients[i+start];
			if (selected[i+start] == 1){
				r = 0.0;
				b = 1.0;
				sel = 1;
			} else {
				r = 1.0;
				b = 0.0;
				sel = 0;
			}
			if (i<remaining/2){
				x = x1;
				y = yinit - (i+2)*0.5;
			} else {
				if (dynamic == 1){
				x = x2+1.0;
				} else {
				x = x2;
				}
				y = yinit - ((i+2)-(remaining/2))*0.5;
			}
			s = 0.2;
			tR = 1.0;
			tB = 1.0;
			tG = 1.0;
			tS = 0.8;
			makeButton(i,x,y,z,s,r,g,b,tS,tR,tG,tB,t,nT,sel,d);
		}
	} else {
		numIngredientsDisplayed = 20;
		for (i=0; i<20;i++){
			t = ingredients[i+start];
			if (selected[i+start] == 1){
				r = 0.0;
				b = 1.0;
				sel = 1;
			} else {
				r = 1.0;
				b = 0.0;
				sel = 0;
			}
			if (i<10){
				x = x1;
				y = yinit - (i+2)*0.5;
			} else {
				if (dynamic == 1){
				x = x2+1.0;
				} else {
				x = x2;
				}
				y = yinit - ((i+2)-(10))*0.5;
			}
			s = 0.2;
			tR = 1.0;
			tB = 1.0;
			tG = 1.0;
			tS = 0.8;
			makeButton(i,x,y,z,s,r,g,b,tS,tR,tG,tB,t,nT,sel,d);
		}
		
		x = x1;
		y = yinit;
		s = 0.2;
		r = 0.5;
		b = 1.0;
		tS = 1.0;
		t = "more ingredients";
		for (a=0;a<5;a++){
			strcpy(nT[a], "");
		}
		d = 0;
		makeButton(i,x,y,z,s,r,g,b,tS,tR,tG,tB,t,nT,sel,d);
		moreDrinks = i;
	}
	if (ingredientsSet != 0){
		i++;
		x = x1;
		y = yinit - 0.5;
		s = 0.2;
		r = 0.5;
		g = 0.0;
		b = 1.0;
		tS = 1.0;
		t = "previous ingredients";
		for (a=0;a<5;a++){
			strcpy(nT[a], "");
		}
		d = 0;
		makeButton(i,x,y,z,s,r,g,b,tS,tR,tG,tB,t,nT,sel,d);
		lastDrinks = i;
	} else {
		lastDrinks = -1;
	}
	i++;
	t = "Select ingredient";
	r = 0.0;
	x = x1;
	y = yinit+0.5;
	s = 0.0;
	tS = 1.5;
	makeButton(i,x,y,z,s,r,g,b,tS,tR,tG,tB,t,nT,sel,d);
	i++;
	if (MYOindex != 0){
		t = "Cancel ingredient";
		b = 0.5;
		x = x1;
		y = yinit-(1+12)*0.5;
		s = 0.3;
		tS = 1.0;
		makeButton(i,x,y,z,s,r,g,b,tS,tR,tG,tB,t,nT,sel,d);
		back2 = i;
		i++;
	} else {
		back2 = -1;
	}
	t = "Cancel Changes";
	b = 0.5;
	x = x1;
	y = yinit-7.3;
	s = 0.3;
	tS = 1.0;
	makeButton(i,x,y,z,s,r,g,b,tS,tR,tG,tB,t,nT,sel,d);
	back = i;
	i++;
	numButtons = i;
}


/*
 * void drawMYOAmounts(int index)
 *
 * make your own option - displays possible amounts
 */
void drawMYOAmounts(int index)
{
	int i;
	int a;
	char nT[5][60];	// the ingredients of the drink
	double x = 0.0;		// x position		
	double y = 0.0;		// y position
	double z = 0.0;		// z position
	double s = 0.0;		// size of the ball (radius)
	double r = 0.0;		// amount of red of ball
	double g = 0.0;		// amount of green of ball
	double b = 0.0;		// amount of blue of ball
	double tS = 0.0;		// size of the ball (radius)
	double tR = 0.0;		// amount of red of ball
	double tG = 0.0;		// amount of green of ball
	double tB = 0.0;		// amount of blue of ball
	char * t = "";		// the name of the drink
	int sel = 0;		// if the ball is selected
	int d = 0;			// position in possible drinks array

	for (a=0; a<5; a++){
			strcpy(nT[a], "");
	}

	strcpy(MYODrink.ingredient[MYOindex],ingredients[index]);
	initBalls();
	for (i=0; i<MAXAMOUNTS;i++){
		t = MYOTextAmounts[i];
		r = 1.0;
		x = x1;
		y = yinit - (i+1)*0.5;
		s = 0.2;
		tR = 1.0;
		tB = 1.0;
		tG = 1.0;
		tS = 1.0;
		makeButton(i,x,y,z,s,r,g,b,tS,tR,tG,tB,t,nT,sel,d);
	}
	i++;
	t = "Select amount";
	x = x1;
	y = yinit+0.5;
	s = 0.0;
	tS = 1.5;
	makeButton(i,x,y,z,s,r,g,b,tS,tR,tG,tB,t,nT,sel,d);
	i++;
	t = ingredients[index];
	x = x1;
	y = yinit;
	tS = 1.2;
	makeButton(i,x,y,z,s,r,g,b,tS,tR,tG,tB,t,nT,sel,d);
	i++;
	t = "Cancel ingredient";
	b = 0.5;
	r = 0.0;
	x = x1;
	y = yinit - (i-1)*0.5;
	s = 0.3;
	tS = 1.0;
	makeButton(i,x,y,z,s,r,g,b,tS,tR,tG,tB,t,nT,sel,d);
	back2 = i;
	i++;
	t = "Cancel Drink";
	b = 0.5;
	r = 0.0;
	x = x1;
	y = yinit-7.3;
	s = 0.3;
	tS = 1.0;
	makeButton(i,x,y,z,s,r,g,b,tS,tR,tG,tB,t,nT,sel,d);
	back = i;
	i++;
	numButtons = i;
}


/*
 * void removeMYOIngredient(int ball)
 *
 * make your own option - removes an ingredient
 */
void removeMYOIngredient(int ball)
{
	int a = ball;

	for (int i=0; i<MAXINGREDIENTS; i++){
		if (strcmp(ingredients[i],MYODrink.ingredient[a]) == 0){
			selected[i] = 0;
		}
	}
	while (strcmp(MYODrink.ingredient[a], "null") != 0){ 
		strcpy(MYODrink.ingredient[a],MYODrink.ingredient[a+1]);
		MYODrink.amount[a] = MYODrink.amount[a+1];
		strcpy(MYODrink.amountString[a],MYODrink.amountString[a+1]);
		strcpy(MYODrink.textIngredientsAmounts[a],MYODrink.textIngredientsAmounts[a+1]);
		a++;
	}
	MYOindex--;
	MYODrink.numIngredients = MYOindex;
}

void increaseMYOIngredient(int ingrIndex, double amountIncreased){

	int decimal,sign;

	MYODrink.amount[ingrIndex] = MYODrink.amount[ingrIndex] + amountIncreased;
	strcpy(MYODrink.amountString[MYOindex],(_fcvt(MYODrink.amount[ingrIndex],0,&decimal,&sign)));
	strcpy(MYODrink.textIngredientsAmounts[ingrIndex],MYODrink.ingredient[ingrIndex]);
	strcat(MYODrink.textIngredientsAmounts[ingrIndex]," ");
	strcat(MYODrink.textIngredientsAmounts[ingrIndex],(_fcvt(MYODrink.amount[ingrIndex],0,&decimal,&sign)));
	strcat(MYODrink.textIngredientsAmounts[ingrIndex],"ml");
}

void decreaseMYOIngredient(int ingrIndex, double amountDecreased){
	int decimal,sign;

	MYODrink.amount[ingrIndex] = MYODrink.amount[ingrIndex] - amountDecreased;
	strcpy(MYODrink.amountString[MYOindex],(_fcvt(MYODrink.amount[ingrIndex],0,&decimal,&sign)));
	strcpy(MYODrink.textIngredientsAmounts[ingrIndex],MYODrink.ingredient[ingrIndex]);
	strcat(MYODrink.textIngredientsAmounts[ingrIndex]," ");
	strcat(MYODrink.textIngredientsAmounts[ingrIndex],(_fcvt(MYODrink.amount[ingrIndex],0,&decimal,&sign)));
	strcat(MYODrink.textIngredientsAmounts[ingrIndex],"ml");

}

/*
 * void setMYOPossibleDrinks(void)
 *
 * make your own option - sets the possible drinks for the dynamic interface
 */
void setMYOPossibleDrinks(void)
{
	int i, numSelected, j, g, a, count;

	numSelected = MYODrink.numIngredients;;
	for(i=0; i<numSelected; i++){
		strcpy(selectedIngredients[i],MYODrink.ingredient[i]);
	}
	j=0;
	g=0;
	while(strcmp(drinks[g].name, "null") != 0){
		if (drinks[g].validDrink == 1){
			count = 0;
			for(i=0; i<numSelected; i++){
				a=0;
				while(strcmp(drinks[g].ingredient[a], "null") != 0){ 
					if (strcmp(drinks[g].ingredient[a], selectedIngredients[i]) == 0)
						count = count++;
					a++;
				}
			}
			if (count == numSelected){
			possibleDrinks[j] = drinks[g];
			possibleDrinks[j].drinkPos = g;
			j++;
			}
		}
		g++;
	}
	possibleDrinks[j] = drinks[g];
	numPossibleDrinks = j;
}





/*
 * void setPossibleDrinks(void)
 *
 * compiles a posssible drinks array
 */
void setPossibleDrinks(int pipeDream)
{
	int a,i,j,g;
	int numSelected, count;

	g = 0;

	if (pipeDream){
		for (i=0; i<=MAXPIPES; i++){
			if(pipeIngredients[i].pipeNum <= MAXPIPES){
				strcpy( selectedIngredients[g], pipeIngredients[i].name);
				g++;
			}
		}
	} else {
		for (i=0; i<MAXINGREDIENTS; i++){
			if(selected[i] == 1){
				strcpy( selectedIngredients[g], ingredients[i]);
				g++;
			}
		}
	}
	numSelected = g;
	j=0;
	if (numSelected!=0){
		g=0;
		while(strcmp(drinks[g].name, "null") != 0){
				count = 0;
				for(i=0; i<numSelected; i++){
					a=0;
					while(strcmp(drinks[g].ingredient[a], "null") != 0){ 
						if (strcmp(drinks[g].ingredient[a], selectedIngredients[i]) == 0){
							count = count++;
						}
						a++;
					}
				}
				if (count == drinks[g].numIngredients){
					possibleDrinks[j] = drinks[g];
					possibleDrinks[j].drinkPos = g;
					j++;
				}
			g++;
		}
	}
	possibleDrinks[j] = drinks[g];
	numPossibleDrinks = j;
}


/*
 * void setAllPossibleDrinks(void)
 *
 * compiles a posssible drinks array with all drinks
 */
void setAllPossibleDrinks(void)
{
	int j,g;
	
	g = 0;
	j=0;
	while(strcmp(drinks[g].name, "null") != 0){
		if (drinks[g].validDrink == 1){
			possibleDrinks[j] = drinks[g];
			possibleDrinks[j].drinkPos = g;
			j++;
		}
		g++;
	}
	possibleDrinks[j] = drinks[g];
	numPossibleDrinks = j;
}

void setAlphabetDrinks(void){
	int i;
	int count;
	int j = 0;

	for (i=0; i<26; i++){
		count = 0;
		while (_strnicmp(alphabet[i],drinks[j].name,1) == 0 &&
			(strcmp(drinks[j].name,"null")!=0)){
			if (count == 0){
				// set start drink for letter
				alphabetInfo[i].startDrink = j;
			}
			count++;
			j++;
		}
		if (count == 0){ // no drinks with that letter
			// set start drink to -1 for letter
			alphabetInfo[i].startDrink = -1;
		}
		// set count for letter
		alphabetInfo[i].numDrinks = count;
		// set stop drink for letter
		alphabetInfo[i].stopDrink = j-1;
	}

}

void setPossibleAlphabetDrinks(void){
	int i;
	int count;
	int j = 0;

	for (i=0; i<26; i++){
		count = 0;
		while (_strnicmp(alphabet[i],possibleDrinks[j].name,1) == 0 &&
			(strcmp(possibleDrinks[j].name,"null")!=0)){
			if (count == 0){
				// set start drink for letter
				alphabetInfo[i].startDrink = j;
			}
			count++;
			j++;
		}
		if (count == 0){ // no drinks with that letter
			// set start drink to -1 for letter
			alphabetInfo[i].startDrink = -1;
		}
		// set count for letter
		alphabetInfo[i].numDrinks = count;
		// set stop drink for letter
		alphabetInfo[i].stopDrink = j-1;
	}

}


/*
 * void setVisibleDrinks(int ball)
 * sets up an array of balls that will be displayed
 * based on similarity to the center ball, which is an argument
 * for this function.
 * also sets up an array of similarities.
 */
void setVisibleDrinks(int ball, int newPos)
{
	int g,a,b,z,count,index, pos,m,check;
	double count2, number,number2,limit;
	int numb;
	int num = numingredients;

	if (ball == -1){
		numVisibleDrinks = 0;
	}
	if (ball != centerballBalls){
		if (ball == -1){
			if (newPos == -1){
				pos = 0;
				initDynamicBalls();
			} else {
				pos = newPos;
			}
		} else {
			centerballDrinks = movingButtons[ball].drinkPos;
			pos = centerballDrinks;
		}
		numVisibleDrinks = 0;
		limit = 1.0;
		numb = 0;
		check = 0;
		while((numb < 10) || (numb > 30)){
			initNewDynamicBalls();
			/*
			 * Determine visible objects and attractions for first center
			 */
			g = 0;
			index = 0;
			if(strcmp(possibleDrinks[pos].name, "null") != 0)
			{
				while(strcmp(possibleDrinks[g].name, "null") != 0)
				{
					a=0;
					count = 0;
					while(strcmp(possibleDrinks[pos].ingredient[a],"null") != 0)
					{
						b = 0;
						while (strcmp(possibleDrinks[g].ingredient[b], "null") != 0){
							if(strcmp(possibleDrinks[g].ingredient[b], possibleDrinks[pos].ingredient[a]) ==0)
							{
								count++;
							}
							b++;
						}
						a++;
					}
					number = (double)(b-1);	// number of ingredients for other ball
					number2 = (double)(a-1); // number of ingredients for center ball
					count2 = (double)count;	// number of same ingredients
					if (numb > 30){
						percent = (count2/number + count2/number2)/2;
					} else {
						percent = count2/number;
					}
					if (percent>limit)
					{
						if (possibleDrinks[g].visible == 1){
							// set position to old position
							newMovingButtons[index].posx = movingButtons[possibleDrinks[g].ballPosition].posx;
							newMovingButtons[index].posy = movingButtons[possibleDrinks[g].ballPosition].posy;
							possibleDrinks[g].newBallPosition = index;
						} else {
							possibleDrinks[g].visible = 1;
							possibleDrinks[g].newBallPosition = index;
						}
						newMovingButtons[index].text = possibleDrinks[g].name;
						for (m = 0; m<5; m++)
							strcpy(newMovingButtons[index].nextText[m],possibleDrinks[g].textIngredients[m]);
						newMovingButtons[index].drinkPos = g;
						newMovingButtons[index].red = (double)possibleDrinks[newMovingButtons[index].drinkPos].numIngredients / 5.0;
						newMovingButtons[index].blue = 1.0 - ((double)possibleDrinks[newMovingButtons[index].drinkPos].numIngredients / 5.0);
						if (g == pos){
							centerballBalls = index;
							newMovingButtons[centerballBalls].center = 1;
						}
						index++;
					} 
					if(percent<=limit)
					{
						possibleDrinks[g].visible = 0;			
					}
					g++;				
				}
			}
			//set the number of visible drinks
			numVisibleDrinks = index;
			numb = numVisibleDrinks;
			if (numPossibleDrinks < 10){
				if (numPossibleDrinks == numVisibleDrinks)
					numb = 10;
			}
			if (check == 1){
				numb = 10;
			}
			if (numb>30){
				limit = limit + 0.05;
				check = 1;
			} else {
				limit = limit - 0.05;
			}

			if (limit<=0){
				limit = 0.05;
				check = 1;
			}
		}
		for (z=0; z<numVisibleDrinks; z++){
			movingButtons[z] = newMovingButtons[z];
			possibleDrinks[movingButtons[z].drinkPos].ballPosition = z;
		}
		for(int l =0; l<MAXDRINKS-1; l++){
			for(int m = 0; m<MAXDRINKS-1; m++){
				otherAttractions[l][m] = 0;
			}
		}
		//determine attractions between the visible objects that are not the center
		for(index = 0; index<numVisibleDrinks-1; index++){
			for(g = index; g<numVisibleDrinks; g++){
				a = 0;
				count = 0;
				while(strcmp(possibleDrinks[movingButtons[index].drinkPos].ingredient[a], "null") != 0){
					b = 0;
					while (strcmp(possibleDrinks[movingButtons[g].drinkPos].ingredient[b], "null") !=0){
						if(strcmp(possibleDrinks[movingButtons[g].drinkPos].ingredient[b], possibleDrinks[movingButtons[index].drinkPos].ingredient[a]) ==0){
							count++;
						}
						b++;
					}
					a++;		
				}
				otherAttractions[index][g] = count/(double)(possibleDrinks[movingButtons[g].drinkPos].numIngredients);
				otherAttractions[g][index] = count/(double)(possibleDrinks[movingButtons[index].drinkPos].numIngredients);
			}
		}
		//some bug, not sure how to fix it properly, but this works
		//otherwise this function changes numingredients
		numingredients = num;
	}
}

void selectAmount(int pipe){
	int i;
	char nT[5][60];	// the ingredients of the drink
	double x = 0.0;		// x position		
	double y = 0.0;		// y position
	double z = 0.0;		// z position
	double s = 0.0;		// size of the ball (radius)
	double r = 0.0;		// amount of red of ball
	double g = 0.0;		// amount of green of ball
	double b = 0.0;		// amount of blue of ball
	double tS = 0.0;		// size of the ball (radius)
	double tR = 0.0;		// amount of red of ball
	double tG = 0.0;		// amount of green of ball
	double tB = 0.0;		// amount of blue of ball
	char * t = "";		// the name of the drink
	int sel = 0;		// if the ball is selected
	int d = 0;			// position in possible drinks array

	initBalls();
	
	tS = 1.0;
	tR = 1.0;
	tG = 1.0;
	tB = 1.0;
	r = 0.5;
	b = 0.5;
	x = x1;
	s = 0.2;
	for (i=0; i<10; i++){
		y = yinit - (i+1)*0.5;
		t = pipeAmounts[i];
		makeButton(i,x,y,z,s,r,g,b,tS,tR,tG,tB,t,nT,sel,d);
	}

	x = x2;
	s = 0.2;
	for (i=10; i<20; i++){
		y = yinit - (i-10+1)*0.5;
		t = pipeAmounts2[i-10];
		makeButton(i,x,y,z,s,r,g,b,tS,tR,tG,tB,t,nT,sel,d);
	}

	
	t = "Set Pipe Amount for";
	s = 0.0;
	x = x1;
	y = yinit + 0.5;
	makeButton(i,x,y,z,s,r,g,b,tS,tR,tG,tB,t,nT,sel,d);
	i++;

	x = x1 + 2.9;
	t = "";
	t = newPipeIngredient.name;
	//y = yinit;
	makeButton(i,x,y,z,s,r,g,b,tS,tR,tG,tB,t,nT,sel,d);
	i++;


	t = "Cancel";
	s = 0.3;
	r = 0.0;
	b = 1.0;
	x = x1;
	y = yinit - 6.0;
	makeButton(i,x,y,z,s,r,g,b,tS,tR,tG,tB,t,nT,sel,d);
	back = i;
	i++;

	numButtons = i;

}

void enterName(int drinkName)	// drinkName =1 if drink, =0 if ingredient
{
	int j,i;
	char nT[5][60];	// the ingredients of the drink
	double x = 0.0;		// x position		
	double y = 0.0;		// y position
	double z = 0.0;		// z position
	double s = 0.0;		// size of the ball (radius)
	double r = 0.0;		// amount of red of ball
	double g = 0.0;		// amount of green of ball
	double b = 0.0;		// amount of blue of ball
	double tS = 0.0;		// size of the ball (radius)
	double tR = 0.0;		// amount of red of ball
	double tG = 0.0;		// amount of green of ball
	double tB = 0.0;		// amount of blue of ball
	char * t = "";		// the name of the drink
	int sel = 0;		// if the ball is selected
	int d = 0;			// position in possible drinks array


	initBalls();
	j=0;
	tS = 1.0;
	tR = 1.0;
	tG = 1.0;
	tB = 1.0;
	r = 0.5;
	b = 0.5;
	for (i=0; i<27; i++){
		if (i<13){
			x = x1 + (i-1)*0.5;
			y = yinit - 1.5;
		} else {
			x = x1 + (i-14)*0.5;
			y = yinit - 2.5;
		}
		t = "";
		s = 0.2;
		makeButton(j,x,y,z,s,r,g,b,tS,tR,tG,tB,t,nT,sel,d);
		j++;
		s = 0.0;
		x = x-0.1;
		t = alphabet[i];
		makeButton(j,x,y,z,s,r,g,b,tS,tR,tG,tB,t,nT,sel,d);
		j++;
	}

	if (drinkName == 1){
		t = "Enter drink name";
	} else {
		t = "Enter ingredient name";
	}
	x = x1;
	y = yinit+0.5;
	s = 0.0;
	makeButton(j,x,y,z,s,r,g,b,tS,tR,tG,tB,t,nT,sel,d);
	j++;

	if (drinkName == 1){
		t = MYODrink.name;
	} else {
		t = newIngredient;
	}
	y = yinit-0.5;
	makeButton(j,x,y,z,s,r,g,b,tS,tR,tG,tB,t,nT,sel,d);
	j++;

	t = "Save new name";
	s = 0.3;
	r = 1.0;
	b = 0.0;
	x = x1;
	y = yinit - 4.0;
	makeButton(j,x,y,z,s,r,g,b,tS,tR,tG,tB,t,nT,sel,d);
	next = j;
	j++;

	t = "Cancel";
	s = 0.3;
	r = 0.0;
	b = 1.0;
	x = x1;
	y = yinit - 5.0;
	makeButton(j,x,y,z,s,r,g,b,tS,tR,tG,tB,t,nT,sel,d);
	back = j;
	j++;

	numButtons = j;

}

/*
 * void drawMYODrink(int index)
 *
 * make your own option - displays drink
 */
void drawMYODrink(int index)
{
	int j,i;
	double checkTotalAmount;
	int a;
	int start, remaining;
	int decimal,sign;
	char nT[5][60];	// the ingredients of the drink
	double x = 0.0;		// x position		
	double y = 0.0;		// y position
	double z = 0.0;		// z position
	double s = 0.0;		// size of the ball (radius)
	double r = 0.0;		// amount of red of ball
	double g = 0.0;		// amount of green of ball
	double b = 0.0;		// amount of blue of ball
	double tS = 0.0;		// size of the ball (radius)
	double tR = 0.0;		// amount of red of ball
	double tG = 0.0;		// amount of green of ball
	double tB = 0.0;		// amount of blue of ball
	char * t = "";		// the name of the drink
	int sel = 0;		// if the ball is selected
	int d = 0;			// position in possible drinks array
	int MYOcheck = 0;
	int MYOcheckIndex = 0;
	int MYOingredientIndex = 0;
	int colourSet;

	for (a=0; a<5; a++){
			strcpy(nT[a], "");
	}
	if (index == -2){
	} else if (index == -1){
		strcpy(MYODrink.ingredient[MYOindex],"null");
		MYODrink.numIngredients = MYOindex;
	} else if (index == -3){	// edit drink
		MYODrink = newChosenDrink;
		MYOindex = MYODrink.numIngredients;
		clearSelected();
		for (i=0; i<MAXINGREDIENTS; i++){
			for (j=0; j<MYODrink.numIngredients; j++){
				if (strcmp(ingredients[i],MYODrink.ingredient[j]) == 0){
					selected[i]=1;
				}
			}
		}
	} else {
		for (i=0; i<MAXINGREDIENTS; i++){
			if (strcmp(MYODrink.ingredient[MYOindex],ingredients[i]) == 0){
				if (selected[i] == 1){
					MYOcheck = 1;
					MYOingredientIndex = i;
					//add amount to that ingredient
				}
			}
		}
		for (i=0; i<MYODrink.numIngredients; i++){
			if (strcmp(MYODrink.ingredient[i],ingredients[MYOingredientIndex]) == 0){
				MYOcheckIndex = i;
			}
		}
		if (MYOcheck == 1){

			MYODrink.amount[MYOcheckIndex] = MYODrink.amount[MYOcheckIndex] + MYOAmounts[index];
			strcpy(MYODrink.amountString[MYOindex],(_fcvt(MYODrink.amount[MYOcheckIndex],0,&decimal,&sign)));
			strcpy(MYODrink.textIngredientsAmounts[MYOcheckIndex],MYODrink.ingredient[MYOcheckIndex]);
			strcat(MYODrink.textIngredientsAmounts[MYOcheckIndex]," ");
			strcat(MYODrink.textIngredientsAmounts[MYOcheckIndex],(_fcvt(MYODrink.amount[MYOcheckIndex],0,&decimal,&sign)));
			strcat(MYODrink.textIngredientsAmounts[MYOcheckIndex],"ml");
		} else {
			MYODrink.amount[MYOindex] = MYOAmounts[index];
			strcpy(MYODrink.amountString[MYOindex],MYOTextAmounts[index]);
			strcpy(MYODrink.textIngredientsAmounts[MYOindex],MYODrink.ingredient[MYOindex]);
			strcat(MYODrink.textIngredientsAmounts[MYOindex]," ");
			strcat(MYODrink.textIngredientsAmounts[MYOindex],MYOTextAmounts[index]);
			for (i=0; i<MAXINGREDIENTS; i++){
				if (strcmp(MYODrink.ingredient[MYOindex],ingredients[i]) == 0){
					selected[i] = 1;
				}
			}
			MYOindex++;
		}
		MYODrink.numIngredients = MYOindex;
		strcpy(MYODrink.ingredient[MYOindex],"null");		
	}
	setMYOPossibleDrinks();
	if(dynamic == 1){
		setVisibleDrinks(-1,0);
	}
	initBalls();
	j=0;
	checkTotalAmount = 0;
	a=0;
	start = MYOset * 10;
	remaining = MYOindex - start;
	while(strcmp(MYODrink.ingredient[a], "null") != 0){
		checkTotalAmount = checkTotalAmount + MYODrink.amount[a];
		a++;
	}
	a = 0;
	if (remaining <= 10){
		numIngredientsDisplayed = remaining;
		moreDrinks = -1;
		while(strcmp(MYODrink.ingredient[a+start], "null") != 0){
			colourSet = a+start;
			while (colourSet >= 6){
				colourSet = colourSet - 6;
			}
			t = "edit";
			tR = 1.0;
			tB = 1.0;
			tG = 1.0;
			tS = 1.0;
			r = 1.0 - colourSet * 0.2;
			b = colourSet * 0.2;
			s = 0.15;
			if (dynamic == 1){
			x = x2 + 2.1;	
			} else {
			x = x2;
			}
			y = yinit - (a+1)*0.5;
			makeButton(j,x,y,z,s,r,g,b,tS,tR,tG,tB,t,nT,sel,d);
			a++;
			j++;
		}
		a = 0;
		while(strcmp(MYODrink.ingredient[a+start], "null") != 0){
			colourSet = a+start;
			while (colourSet >= 6){
				colourSet = colourSet - 6;
			}
			tR = 1.0 - colourSet * 0.2;
			tB = colourSet * 0.2;
			tG = 0.0;
			tS = 1.2;
			s = 0.0;
			t = MYODrink.textIngredientsAmounts[a+start];
			x = x1 - 0.5;
			y = yinit - (a+1)*0.5;
			makeButton(j,x,y,z,s,r,g,b,tS,tR,tG,tB,t,nT,sel,d);
			a++;
			j++;
		}
	} else {
		numIngredientsDisplayed = 9;
		for (a=0; a<10; a++){
			colourSet = a+start;
			while (colourSet >= 6){
				colourSet = colourSet - 6;
			}

			t = "edit";
			tR = 1.0;
			tB = 1.0;
			tG = 1.0;
			tS = 1.0;
			r = 1.0 - colourSet * 0.2;
			b = colourSet * 0.2;
			s = 0.15;
			if (dynamic == 1){
			x = x2 + 2.1;	
			} else {
			x = x2;
			}
			y = yinit - (a+1)*0.5;
			makeButton(j,x,y,z,s,r,g,b,tS,tR,tG,tB,t,nT,sel,d);
			j++;
		}
		a = 0;
		
		for (a=0; a<10; a++){
			colourSet = a+start;
			while (colourSet >= 6){
				colourSet = colourSet - 6;
			}
			tR = 1.0 - colourSet * 0.2;
			tB = colourSet * 0.2;
			tG = 0.0;
			tS = 1.2;
			s = 0.0;
			t = MYODrink.textIngredientsAmounts[a+start];
			x = x1 - 0.5;
			y = yinit - (a+1)*0.5;
			makeButton(j,x,y,z,s,r,g,b,tS,tR,tG,tB,t,nT,sel,d);
			j++;
		}	
		s = 0.2;
		r = 0.0;
		b = 1.0;
		tR = 1.0;
		tB = 1.0;
		tG = 1.0;
		tS = 1.0;
		t = "more ingredients";
		if (dynamic == 1){
		x = x2+1.4;
		} else {
		x = x2;
		}
		y = yinit - 5.5;
		makeButton(j,x,y,z,s,r,g,b,tS,tR,tG,tB,t,nT,sel,d);
		moreDrinks = j;
		j++;
	}
	if (MYOset !=0){
		s = 0.2;
		r = 0.0;
		b = 1.0;
		tR = 1.0;
		tG = 1.0;
		tB = 1.0;
		tS = 1.0;
		t = "last ingredients";
		if (dynamic == 1){
		x = x2+1.4;
		} else {
		x = x2;
		}
		y = yinit - 6.0;
		makeButton(j,x,y,z,s,r,g,b,tS,tR,tG,tB,t,nT,sel,d);
		lastDrinks = j;
		j++;
	} else {
		lastDrinks = -1;
	}
	t = MYODrink.name;
	s = 0.0;
	tR = 1.0;
	tB = 0.5;
	tG = 0.0;
	tS = 1.5;
	x = x1-1.2;
	y = yinit;
	makeButton(j,x,y,z,s,r,g,b,tS,tR,tG,tB,t,nT,sel,d);
	j++;
	if (checkTotalAmount <= (DRINKSIZE - 90)){
		t = "Add another";
		tR = 1.0;
		tB = 1.0;
		tG = 1.0;
		tS = 1.0;
		b = 0.0;
		r = 0.5;
		s = 0.3;
		tS = 1.0;
		x = x1;
		if (MYOindex < 10){
			y = yinit - (2+MYODrink.numIngredients)*0.5;
		} else {
			y = yinit - 5.5;
		}
		makeButton(j,x,y,z,s,r,g,b,tS,tR,tG,tB,t,nT,sel,d);
		next2 = j;
		j++;
		s = 0.0;
		x = x+0.4;
		y = y-0.2;
		t = "ingredient";
		makeButton(j,x,y,z,s,r,g,b,tS,tR,tG,tB,t,nT,sel,d);
		j++;
	} else {
		next2 = -1;
	}

	tR = 1.0;
	tB = 1.0;
	tG = 1.0;
	tS = 1.0;
	b = 0.5;
	r = 0.5;
	s = 0.3;

	x = x1;
	t = "Cancel Changes";
	r = 0.0;
	y = yinit - 7.3;
	makeButton(j,x,y,z,s,r,g,b,tS,tR,tG,tB,t,nT,sel,d);
	back = j;
	j++;

	tR = 1.0;
	tB = 1.0;
	tG = 1.0;
	tS = 1.0;
	b = 0.5;
	r = 0.5;
	s = 0.3;
	if (MYODrink.numIngredients != 0){
		t = "Save Changes";
		x = x1;
		if (MYOindex < 10){
			y = yinit - (4+MYODrink.numIngredients)*0.5;
		} else {
			y = yinit - 6.5;
		}
		makeButton(j,x,y,z,s,r,g,b,tS,tR,tG,tB,t,nT,sel,d);
		next = j;
		j++;
	} else {
		next = -1;
	}
	numButtons = j;


}

void drawEditMYOIngredient(){
	int i;
	int a;
	double checkTotalAmount;
	char nT[5][60];	// the ingredients of the drink
	double x = 0.0;		// x position		
	double y = 0.0;		// y position
	double z = 0.0;		// z position
	double s = 0.0;		// size of the ball (radius)
	double r = 0.0;		// amount of red of ball
	double g = 0.0;		// amount of green of ball
	double b = 0.0;		// amount of blue of ball
	double tS = 0.0;		// size of the ball (radius)
	double tR = 0.0;		// amount of red of ball
	double tG = 0.0;		// amount of green of ball
	double tB = 0.0;		// amount of blue of ball
	char * t = "";		// the name of the drink
	int sel = 0;		// if the ball is selected
	int d = 0;			// position in possible drinks array

	for (a=0; a<5; a++){
			strcpy(nT[a], "");
	}
	initBalls();
	i=0;
	t = MYODrink.name;
	y = yinit + 0.5;
	tR = 1.0;
	tB = 0.5;
	tG = 0.0;
	tS = 1.5;
	x = x1-1.2;
	makeButton(i,x,y,z,s,r,g,b,tS,tR,tG,tB,t,nT,sel,d);
	i++;
	t = MYODrink.textIngredientsAmounts[editIngredientIndex];
	x = x1-0.5;
	y = yinit;
	tS = 1.2;
	tR = 1.0;
	tG = 1.0;
	tB = 1.0;
	makeButton(i,x,y,z,s,r,g,b,tS,tR,tG,tB,t,nT,sel,d);
	i++;
	checkTotalAmount = 0;
	a = 0;
	while(strcmp(MYODrink.ingredient[a], "null") != 0){
		checkTotalAmount = checkTotalAmount + MYODrink.amount[a];
		a++;
	}
	if (checkTotalAmount <= (DRINKSIZE - 90)){
	t = "Increase amount by:";
	x = x1 - 0.5;
	y = yinit - 1.0;
	makeButton(i,x,y,z,s,r,g,b,tS,tR,tG,tB,t,nT,sel,d);
	i++;
	}
	if (MYODrink.amount[editIngredientIndex] > 15){
	t = "Decrease amount by:";
	y = yinit - 1.5 - MAXAMOUNTS * 0.5;
	makeButton(i,x,y,z,s,r,g,b,tS,tR,tG,tB,t,nT,sel,d);
	i++;
	}
	t = "Remove";
	b = 0.5;
	r = 0.0;
	x = x1;
	y = yinit-0.5;
	s = 0.2;
	makeButton(i,x,y,z,s,r,g,b,tS,tR,tG,tB,t,nT,sel,d);
	next = i;
	i++;

	addAmountStart = i;
	if (checkTotalAmount <= (DRINKSIZE - 90)){
	for (a=0; a<MAXAMOUNTS;a++){
		t = MYOTextAmounts[a];
		r = 1.0;
		b = 0.0;
		y = yinit - (a+3)*0.5;
		s = 0.2;
		tR = 1.0;
		tB = 1.0;
		tG = 1.0;
		tS = 1.0;
		makeButton(i,x,y,z,s,r,g,b,tS,tR,tG,tB,t,nT,sel,d);
		i++;
	}
	}
	addAmountStop = i;

	removeAmountStart = i;
	for (a=0; a<MAXAMOUNTS;a++){
		if ((MYODrink.amount[editIngredientIndex]-5) > MYOAmounts[a]){
			t = MYOTextAmounts[a];
			r = 1.0;
			b = 0.0;
			y = yinit - 0.5 - (MAXAMOUNTS+a+3) * 0.5;
			s = 0.2;
			tR = 1.0;
			tB = 1.0;
			tG = 1.0;
			tS = 1.0;
			makeButton(i,x,y,z,s,r,g,b,tS,tR,tG,tB,t,nT,sel,d);
			i++;
		}
	}
	removeAmountStop = i;

	t = "Cancel";
	b = 0.5;
	r = 0.0;
	x = x1;
	y = yinit-7.3;
	s = 0.3;
	tS = 1.0;
	makeButton(i,x,y,z,s,r,g,b,tS,tR,tG,tB,t,nT,sel,d);
	back = i;
	i++;
	numButtons = i;
}

/*
 * void moveBalls (void)
 *
 * for each of the balls, determine the new position from the forces acting
 * on the ball, and redraw the ball in this position
 */
void moveBalls (void)
{
	for (int j = 0; j<numVisibleDrinks; j++) {
		if (movingButtons[j].center == 0){
			movingButtons[j].oldposx = movingButtons[j].posx;
			movingButtons[j].oldposy = movingButtons[j].posy;
			// initialise the repulsive force
			repforce = 0.0;
			reptheta = 0;
			// add up all of the repulsive forces
			for (int a=0; a<numVisibleDrinks; a++){
				if (a != j){
					// first make sure that the balls are not in the same x or y position
					// if they are, add a random amount to the position (between -0.5 and 0.5)
					if (movingButtons[j].posx == movingButtons[a].posx){
						num = rand();
						num = (num / 32767) - 0.5;
						movingButtons[j].posx = movingButtons[j].posx + num;
					}
					if (movingButtons[j].posy == movingButtons[a].posy) {
						num = rand();
						num = (num / 32767) - 0.5;
						movingButtons[j].posy = movingButtons[j].posy + num;
					}
					// determine the distance between the two balls
					distance = sqrt((movingButtons[j].posx - movingButtons[a].posx) * (movingButtons[j].posx - movingButtons[a].posx) 
						+ (movingButtons[j].posy - movingButtons[a].posy) * (movingButtons[j].posy - movingButtons[a].posy));
					// determine the repulsive force and direction of the force
					repforce1 = e / (fabs(distance) * distance);
					reptheta1 = atan2(movingButtons[j].posy - movingButtons[a].posy, movingButtons[j].posx - movingButtons[a].posx);
					// determine the x and y components of the force
					repforcex = repforce * cos(reptheta) + repforce1 * cos(reptheta1);
					repforcey = repforce * sin(reptheta) + repforce1 * sin(reptheta1);
					// add to the sum of the repulsive forces so far
					repforce = sqrt((repforcex * repforcex) + (repforcey * repforcey));
					reptheta = atan2(repforcey,repforcex);
				}
			}
			// work out force, acceleration, velocity and position
			// determine attraction to center based on distance to center
			distance = sqrt ((movingButtons[j].posx * movingButtons[j].posx) + (movingButtons[j].posy * movingButtons[j].posy));
			if (j<centerballBalls){
				attrforce = -((k*otherAttractions[j][centerballBalls])*distance);
			}else{
				attrforce = -((k*otherAttractions[centerballBalls][j])*distance);
			}
			attrtheta = atan2(movingButtons[j].posy,movingButtons[j].posx);
			//determine attraction to other balls based on distance
			for(int l = 0; l<numVisibleDrinks; l++){
				if(l!=j){
					if(otherAttractions[j][l] != 0){
						// first make sure that the balls are not in the same x or y position
						// if they are, add a random amount to the position (between -0.5 and 0.5)
						if (movingButtons[j].posx == movingButtons[a].posx){
							num = rand();
							num = (num / 32767) - 0.5;
							movingButtons[j].posx = movingButtons[j].posx + num;
						}
						if (movingButtons[j].posy == movingButtons[a].posy) {
							num = rand();
							num = (num / 32767) - 0.5;
							movingButtons[j].posy = movingButtons[j].posy + num;
						}
						// determine the distance between the two balls
						distance = sqrt((movingButtons[j].posx - movingButtons[l].posx) * (movingButtons[j].posx - movingButtons[l].posx) 
							+ (movingButtons[j].posy - movingButtons[l].posy) * (movingButtons[j].posy - movingButtons[l].posy));
						// determine the repulsive force and direction of the force
						if(j<l){
							attrforce1 = -((k*otherAttractions[j][l])*distance);
						}else{
							attrforce1 = -((k*otherAttractions[l][j])*distance);
						}
						attrtheta1 = atan2(movingButtons[j].posy - movingButtons[l].posy, movingButtons[j].posx - movingButtons[l].posx);
						// determine the x and y components of the force
						attrforcex = attrforce * cos(attrtheta) + attrforce1 * cos(attrtheta1);
						attrforcey = attrforce * sin(attrtheta) + attrforce1 * sin(attrtheta1);
						// add to the sum of the repulsive forces so far
						attrforce = sqrt((attrforcex * attrforcex) + (attrforcey * attrforcey));
						attrtheta = atan2(attrforcey,attrforcex);
					}
				}	
			}
			// determine damping force based on current velocity
			velocity = sqrt((movingButtons[j].velx * movingButtons[j].velx) + (movingButtons[j].vely * movingButtons[j].vely));
			dampforce = -(d*velocity);
			damptheta = atan2(movingButtons[j].vely,movingButtons[j].velx);
			// determine the x and y components of the force on the ball
			forcex = attrforce * cos(attrtheta) + dampforce * cos(damptheta) + repforce * cos(reptheta);
			forcey = attrforce * sin(attrtheta) + dampforce * sin(damptheta) + repforce * sin(reptheta);
			// set the force and direction for the ball
			movingButtons[j].force = sqrt((forcex * forcex) + (forcey * forcey));
			movingButtons[j].forcetheta = atan2(forcey,forcex);
			while (movingButtons[j].force > 1000){
				movingButtons[j].force = movingButtons[j].force / 10;
			}
			// set the acceleration for the ball
			movingButtons[j].accx = movingButtons[j].force * cos(movingButtons[j].forcetheta) / movingButtons[j].mass;
			movingButtons[j].accy = movingButtons[j].force * sin(movingButtons[j].forcetheta) / movingButtons[j].mass;
			// set the velocity for the ball
			movingButtons[j].velx = movingButtons[j].velx + movingButtons[j].accx * timestep;
			movingButtons[j].vely = movingButtons[j].vely + movingButtons[j].accy * timestep;
			// set the new position of the ball
			movingButtons[j].posx = movingButtons[j].posx + movingButtons[j].velx * timestep;
			movingButtons[j].posy = movingButtons[j].posy + movingButtons[j].vely * timestep;				  
		} else {
			// put the center ball in the center position
			distance = sqrt ((movingButtons[j].posx * movingButtons[j].posx) + (movingButtons[j].posy * movingButtons[j].posy));
			if (distance > 0.05){
				attrforce = -(6*k*distance);
				attrtheta = atan2(movingButtons[j].posy,movingButtons[j].posx);
				// determine the x and y components of the force on the ball
				forcex = attrforce * cos(attrtheta);
				forcey = attrforce * sin(attrtheta);
				// set the force and direction for the ball
				movingButtons[j].force = sqrt((forcex * forcex) + (forcey * forcey));
				movingButtons[j].forcetheta = atan2(forcey,forcex);
				while (movingButtons[j].force > 1000){
					movingButtons[j].force = movingButtons[j].force / 10;
				}
				// set the acceleration for the ball
				movingButtons[j].accx = movingButtons[j].force * cos(movingButtons[j].forcetheta) / movingButtons[j].mass;
				movingButtons[j].accy = movingButtons[j].force * sin(movingButtons[j].forcetheta) / movingButtons[j].mass;
				// set the velocity for the ball
				movingButtons[j].velx = movingButtons[j].velx + movingButtons[j].accx * timestep;
				movingButtons[j].vely = movingButtons[j].vely + movingButtons[j].accy * timestep;
				// set the new position of the ball
				movingButtons[j].posx = movingButtons[j].posx + movingButtons[j].velx * timestep;
				movingButtons[j].posy = movingButtons[j].posy + movingButtons[j].vely * timestep;				  
			} else {
				movingButtons[j].posx = 0.0;
				movingButtons[j].posy = 0.0;
			}
		}
	}
	display();
}


/*
 * void showDrinks(int num)
 * 
 * show the possible drinks
 */
void showDrinks(int num, int alphabetOff)
{
	int j,l;
	int start, remaining;
	int a;
	int k;
	char nT[5][60];	// the ingredients of the drink
	double x = 0.0;		// x position		
	double y = 0.0;		// y position
	double z = 0.0;		// z position
	double s = 0.0;		// size of the ball (radius)
	double r = 0.0;		// amount of red of ball
	double g = 0.0;		// amount of green of ball
	double b = 0.0;		// amount of blue of ball
	double tS = 0.0;		// size of the ball (radius)
	double tR = 0.0;		// amount of red of ball
	double tG = 0.0;		// amount of green of ball
	double tB = 0.0;		// amount of blue of ball
	char * t = "";		// the name of the drink
	int sel = 0;		// if the ball is selected
	int d = 0;			// position in possible drinks array

	for (a=0; a<5; a++){
			strcpy(nT[a], "");
	}
	initBalls();
	start = 8 * set;
	l=0;
	remaining = numDrinks - start;
	y = yinit - 1.0;
	if (remaining <= 8){
		for (j=l; j<remaining; j++){
			x = x1;
			s = 0.2;
			r = 1.0;
			tS = 1.0;
			tR = 1.0;
			tG = 1.0;
			tB = 1.0;
			t = drinks[j+start].name;
			for (a=0;a<5;a++){
				strcpy(nT[a], drinks[j+start].textIngredients[a]);
			}
			d = j+start;
			makeButton(j,x,y,z,s,r,g,b,tS,tR,tG,tB,t,nT,sel,d);
			y = y - 0.4 - (1+drinks[j+start].numlines)*0.2;
		}	
		numDrinksDisplayed = j;
		moreDrinks = -1;
	} else {
		for (j=l; j<8; j++){
			x = x1;
			s = 0.2;
			r = 1.0;
			tS = 1.0;
			tR = 1.0;
			tG = 1.0;
			tB = 1.0;
			t = drinks[j+start].name;
			for (a=0;a<5;a++){
				strcpy(nT[a], drinks[j+start].textIngredients[a]);
			}
			d = j+start;
			makeButton(j,x,y,z,s,r,g,b,tS,tR,tG,tB,t,nT,sel,d);
			y = y - 0.4 - (1+drinks[j+start].numlines)*0.2;
		}	
		numDrinksDisplayed = j;
		j++;
		x = x1;
		y = yinit;
		s = 0.2;
		r = 0.5;
		b = 1.0;
		tS = 1.0;
		t = "next 8";
		for (a=0;a<5;a++){
			strcpy(nT[a], "");
		}
		d = 0;
		makeButton(j,x,y,z,s,r,g,b,tS,tR,tG,tB,t,nT,sel,d);
		moreDrinks = j;
	}
	if (set != 0){
		j++;
		x = x1;
		y = yinit - 0.5;
		s = 0.2;
		r = 0.5;
		g = 0.0;
		b = 1.0;
		tS = 1.0;
		t = "previous 8";
		for (a=0;a<5;a++){
			strcpy(nT[a], "");
		}
		d = 0;
		makeButton(j,x,y,z,s,r,g,b,tS,tR,tG,tB,t,nT,sel,d);
		lastDrinks = j;
	} else {
		lastDrinks = -1;
	}
	if (numPossibleDrinks == 0){
		x = x1;
		if (dynamic == 1){
			y = yinit-0.5;
		} else {
			y = yinit+0.5;
		}
		s = 0.0;
		r = 0.0;
		g = 0.0;
		b = 1.0;
		tR = 1.0;
		tB = 1.0;
		tG = 1.0;
		tS = 1.5;
		t = "No drinks with";
		for (a=0;a<5;a++){
			strcpy(nT[a], "");
		}
		d = 0;
		makeButton(j,x,y,z,s,r,g,b,tS,tR,tG,tB,t,nT,sel,d);
		j++;
		y = y-0.5;
		t = "these ingredients";
		makeButton(j,x,y,z,s,r,g,b,tS,tR,tG,tB,t,nT,sel,d);

	} else {
		j++;
		x = x1;
		y = yinit+0.5;
		s = 0.0;
		r = 0.0;
		g = 0.0;
		b = 1.0;
		tS = 1.5;
		t = "Drinks Database";
		for (a=0;a<5;a++){
			strcpy(nT[a], "");
		}
		d = 0;
		makeButton(j,x,y,z,s,r,g,b,tS,tR,tG,tB,t,nT,sel,d);
	}

	j++;
	x = x2;
	y = yinit;
	s = 0.3;
	r = 0.0;
	g = 0.0;
	b = 0.5;
	tS = 1.0;
	t = "Add Drink";
	for (a=0;a<5;a++){
		strcpy(nT[a], "");
	}
	d = 0;
	makeButton(j,x,y,z,s,r,g,b,tS,tR,tG,tB,t,nT,sel,d);
	next = j;
	j++;

	x = x1;
	y = yinit-7.3;
	s = 0.3;
	r = 0.0;
	g = 0.0;
	b = 0.5;
	tS = 1.0;
	t = "Main menu";
	for (a=0;a<5;a++){
		strcpy(nT[a], "");
	}
	d = 0;
	makeButton(j,x,y,z,s,r,g,b,tS,tR,tG,tB,t,nT,sel,d);
	back = j;
	j++;


	
	r = 0.5;

	if ((alphabetOff == 0) && (numPossibleDrinks > 0)){
	k = 0;
	for (int i=0; i<26; i++){
		if (alphabetInfo[i].numDrinks != 0){
			if (k<13){
				x = x1 - 1.0;
				y = yinit - k*0.5;
			} else {
				x = x1 - 0.5;
				y = yinit - (k-13)*0.5;
			}
			k++;
			t = "";
			s = 0.2;
			makeButton(j,x,y,z,s,r,g,b,tS,tR,tG,tB,t,nT,sel,d);
			j++;
			s = 0.0;
			t = alphabet[i];
			x = x-0.1;
			makeButton(j,x,y,z,s,r,g,b,tS,tR,tG,tB,t,nT,sel,d);
			j++;
		}
	}
	numLetters = k;
	}
	numButtons = j;
}

/*
 * void showDrinks(int num)
 * 
 * show the possible drinks
 */
void showPossibleDrinks(int num, int alphabetOff)
{
	int j,l;
	int start, remaining;
	int a;
	int k;
	char nT[5][60];	// the ingredients of the drink
	double x = 0.0;		// x position		
	double y = 0.0;		// y position
	double z = 0.0;		// z position
	double s = 0.0;		// size of the ball (radius)
	double r = 0.0;		// amount of red of ball
	double g = 0.0;		// amount of green of ball
	double b = 0.0;		// amount of blue of ball
	double tS = 0.0;		// size of the ball (radius)
	double tR = 0.0;		// amount of red of ball
	double tG = 0.0;		// amount of green of ball
	double tB = 0.0;		// amount of blue of ball
	char * t = "";		// the name of the drink
	int sel = 0;		// if the ball is selected
	int d = 0;			// position in possible drinks array

	for (a=0; a<5; a++){
			strcpy(nT[a], "");
	}
	initBalls();
	start = 8 * set;
	l=0;
	remaining = numPossibleDrinks - start;
	y = yinit - 1.0;
	if (remaining <= 8){
		for (j=l; j<remaining; j++){
			x = x1;
			s = 0.2;
			r = 1.0;
			tS = 1.0;
			tR = 1.0;
			tG = 1.0;
			tB = 1.0;
			t = possibleDrinks[j+start].name;
			for (a=0;a<5;a++){
				strcpy(nT[a], possibleDrinks[j+start].textIngredients[a]);
			}
			d = j+start;
			makeButton(j,x,y,z,s,r,g,b,tS,tR,tG,tB,t,nT,sel,d);
			y = y - 0.4 - (1+possibleDrinks[j+start].numlines)*0.2;
		}	
		numDrinksDisplayed = j;
		moreDrinks = -1;
	} else {
		for (j=l; j<8; j++){
			x = x1;
			s = 0.2;
			r = 1.0;
			tS = 1.0;
			tR = 1.0;
			tG = 1.0;
			tB = 1.0;
			t = possibleDrinks[j+start].name;
			for (a=0;a<5;a++){
				strcpy(nT[a], possibleDrinks[j+start].textIngredients[a]);
			}
			d = j+start;
			makeButton(j,x,y,z,s,r,g,b,tS,tR,tG,tB,t,nT,sel,d);
			y = y - 0.4 - (1+possibleDrinks[j+start].numlines)*0.2;
		}	
		numDrinksDisplayed = j;
		j++;
		x = x1;
		y = yinit;
		s = 0.2;
		r = 0.5;
		b = 1.0;
		tS = 1.0;
		t = "next 8";
		for (a=0;a<5;a++){
			strcpy(nT[a], "");
		}
		d = 0;
		makeButton(j,x,y,z,s,r,g,b,tS,tR,tG,tB,t,nT,sel,d);
		moreDrinks = j;
	}
	if (set != 0){
		j++;
		x = x1;
		y = yinit - 0.5;
		s = 0.2;
		r = 0.5;
		g = 0.0;
		b = 1.0;
		tS = 1.0;
		t = "previous 8";
		for (a=0;a<5;a++){
			strcpy(nT[a], "");
		}
		d = 0;
		makeButton(j,x,y,z,s,r,g,b,tS,tR,tG,tB,t,nT,sel,d);
		lastDrinks = j;
	} else {
		lastDrinks = -1;
	}
	if (numPossibleDrinks == 0){
		x = x1;
		if (dynamic == 1){
			y = yinit-0.5;
		} else {
			y = yinit+0.5;
		}
		s = 0.0;
		r = 0.0;
		g = 0.0;
		b = 1.0;
		tR = 1.0;
		tB = 1.0;
		tG = 1.0;
		tS = 1.5;
		t = "No drinks with";
		for (a=0;a<5;a++){
			strcpy(nT[a], "");
		}
		d = 0;
		makeButton(j,x,y,z,s,r,g,b,tS,tR,tG,tB,t,nT,sel,d);
		j++;
		y = y-0.5;
		t = "these ingredients";
		makeButton(j,x,y,z,s,r,g,b,tS,tR,tG,tB,t,nT,sel,d);

	} else {
		j++;
		x = x1;
		y = yinit+0.5;
		s = 0.0;
		r = 0.0;
		g = 0.0;
		b = 1.0;
		tS = 1.5;
		t = "Drinks Available";
		for (a=0;a<5;a++){
			strcpy(nT[a], "");
		}
		d = 0;
		makeButton(j,x,y,z,s,r,g,b,tS,tR,tG,tB,t,nT,sel,d);
	}

	j++;

	x = x1;
	y = yinit-7.3;
	s = 0.3;
	r = 0.0;
	g = 0.0;
	b = 0.5;
	tS = 1.0;
	t = "Main menu";
	for (a=0;a<5;a++){
		strcpy(nT[a], "");
	}
	d = 0;
	makeButton(j,x,y,z,s,r,g,b,tS,tR,tG,tB,t,nT,sel,d);
	back = j;
	j++;


	
	r = 0.5;

	if ((alphabetOff == 0) && (numPossibleDrinks > 0)){
	k = 0;
	for (int i=0; i<26; i++){
		if (alphabetInfo[i].numDrinks != 0){
			if (k<13){
				x = x1 - 1.0;
				y = yinit - k*0.5;
			} else {
				x = x1 - 0.5;
				y = yinit - (k-13)*0.5;
			}
			k++;
			t = "";
			s = 0.2;
			makeButton(j,x,y,z,s,r,g,b,tS,tR,tG,tB,t,nT,sel,d);
			j++;
			s = 0.0;
			t = alphabet[i];
			x = x-0.1;
			makeButton(j,x,y,z,s,r,g,b,tS,tR,tG,tB,t,nT,sel,d);
			j++;
		}
	}
	numLetters = k;
	}
	numButtons = j;
}


/*
 * void showLetterDrinks(int num)
 * 
 * show the possible drinks
 */
void showLetterDrinks(int letterIndex, int num)
{
	int j,l;
	int start, remaining;
	int a;
	int k;
	char nT[5][60];	// the ingredients of the drink
	double x = 0.0;		// x position		
	double y = 0.0;		// y position
	double z = 0.0;		// z position
	double s = 0.0;		// size of the ball (radius)
	double r = 0.0;		// amount of red of ball
	double g = 0.0;		// amount of green of ball
	double b = 0.0;		// amount of blue of ball
	double tS = 0.0;		// size of the ball (radius)
	double tR = 0.0;		// amount of red of ball
	double tG = 0.0;		// amount of green of ball
	double tB = 0.0;		// amount of blue of ball
	char * t = "";		// the name of the drink
	int sel = 0;		// if the ball is selected
	int d = 0;			// position in possible drinks array


	for (a=0; a<5; a++){
			strcpy(nT[a], "");
	}
	initBalls();
	start = 8 * num + alphabetInfo[letterIndex].startDrink;
	l=0;
	remaining = alphabetInfo[letterIndex].numDrinks - 8*num;
	y = yinit-1.0;
	tR = 1.0;
	tG = 1.0;
	tB = 1.0;
	if (remaining <= 8){
		for (j=l; j<remaining; j++){
			x = x1;
			s = 0.2;
			r = 1.0;
			tS = 1.0;
			tR = 1.0;
			tG = 1.0;
			tB = 1.0;
			t = drinks[j+start].name;
			for (a=0;a<5;a++){
				strcpy(nT[a], drinks[j+start].textIngredients[a]);
			}
			d = j+start;
			makeButton(j,x,y,z,s,r,g,b,tS,tR,tG,tB,t,nT,sel,d);
			y = y - 0.4 - (1+drinks[j+start].numlines)*0.2;
		}	
		numDrinksDisplayed = j;
		moreDrinks = -1;
	} else {
		for (j=l; j<8; j++){
			x = x1;
			s = 0.2;
			r = 1.0;
			tS = 1.0;
			tR = 1.0;
			tG = 1.0;
			tB = 1.0;
			t = drinks[j+start].name;
			for (a=0;a<5;a++){
				strcpy(nT[a], drinks[j+start].textIngredients[a]);
			}
			d = j+start;
			makeButton(j,x,y,z,s,r,g,b,tS,tR,tG,tB,t,nT,sel,d);
			y = y - 0.4 - (1+drinks[j+start].numlines)*0.2;
		}	
		numDrinksDisplayed = j;
		j++;
		x = x1;
		y = yinit;
		s = 0.2;
		r = 0.5;
		b = 1.0;
		tS = 1.0;
		t = "next 8";
		for (a=0;a<5;a++){
			strcpy(nT[a], "");
		}
		d = 0;
		makeButton(j,x,y,z,s,r,g,b,tS,tR,tG,tB,t,nT,sel,d);
		moreDrinks = j;
	}
	if (set != 0){
		j++;
		x = x1;
		y = yinit - 0.5;
		s = 0.2;
		r = 0.5;
		g = 0.0;
		b = 1.0;
		tS = 1.0;
		t = "previous 8";
		for (a=0;a<5;a++){
			strcpy(nT[a], "");
		}
		d = 0;
		makeButton(j,x,y,z,s,r,g,b,tS,tR,tG,tB,t,nT,sel,d);
		lastDrinks = j;
	} else {
		lastDrinks = -1;
	}
	if (numDrinks == 0){
		x = x1;
		if (dynamic == 1){
			y = yinit-0.5;
		} else {
			y = yinit+0.5;
		}
		s = 0.0;
		r = 0.0;
		g = 0.0;
		b = 1.0;
		tR = 1.0;
		tB = 1.0;
		tG = 1.0;
		tS = 1.5;
		t = "No drinks with";
		for (a=0;a<5;a++){
			strcpy(nT[a], "");
		}
		d = 0;
		makeButton(j,x,y,z,s,r,g,b,tS,tR,tG,tB,t,nT,sel,d);
		j++;
		y = y-0.5;
		t = "these ingredients";
		makeButton(j,x,y,z,s,r,g,b,tS,tR,tG,tB,t,nT,sel,d);
	} else {
		j++;
		x = x1;
		y = yinit+0.5;
		s = 0.0;
		r = 0.0;
		g = 0.0;
		b = 1.0;
		tS = 1.5;
		t = "Drinks Database";
		for (a=0;a<5;a++){
			strcpy(nT[a], "");
		}
		d = 0;
		makeButton(j,x,y,z,s,r,g,b,tS,tR,tG,tB,t,nT,sel,d);
	}
	j++;
	x = x1;
	y = yinit-7.3;
	s = 0.3;
	r = 0.0;
	g = 0.0;
	b = 0.5;
	tS = 1.0;
	t = "Cancel";
	for (a=0;a<5;a++){
		strcpy(nT[a], "");
	}
	d = 0;
	makeButton(j,x,y,z,s,r,g,b,tS,tR,tG,tB,t,nT,sel,d);
	back = j;
	j++;


	
	r = 0.5;

	k = 0;
	for (int i=0; i<26; i++){
		if (alphabetInfo[i].numDrinks != 0){
			if (k<13){
				x = x1 - 1.0;
				y = yinit - k*0.5;
			} else {
				x = x1 - 0.5;
				y = yinit - (k-13)*0.5;
			}

			k++;
			t = "";
			s = 0.2;
			makeButton(j,x,y,z,s,r,g,b,tS,tR,tG,tB,t,nT,sel,d);
			j++;
			s = 0.0;
			t = alphabet[i];
			x = x-0.1;
			makeButton(j,x,y,z,s,r,g,b,tS,tR,tG,tB,t,nT,sel,d);
			j++;
		}
	}
	numLetters = k;

	numButtons = j;
}

/*
 * void showLetterDrinks(int num)
 * 
 * show the possible drinks
 */
void showPossibleLetterDrinks(int letterIndex, int num)
{
	int j,l;
	int start, remaining;
	int a;
	int k;
	char nT[5][60];	// the ingredients of the drink
	double x = 0.0;		// x position		
	double y = 0.0;		// y position
	double z = 0.0;		// z position
	double s = 0.0;		// size of the ball (radius)
	double r = 0.0;		// amount of red of ball
	double g = 0.0;		// amount of green of ball
	double b = 0.0;		// amount of blue of ball
	double tS = 0.0;		// size of the ball (radius)
	double tR = 0.0;		// amount of red of ball
	double tG = 0.0;		// amount of green of ball
	double tB = 0.0;		// amount of blue of ball
	char * t = "";		// the name of the drink
	int sel = 0;		// if the ball is selected
	int d = 0;			// position in possible drinks array


	for (a=0; a<5; a++){
			strcpy(nT[a], "");
	}
	initBalls();
	start = 8 * num + alphabetInfo[letterIndex].startDrink;
	l=0;
	remaining = alphabetInfo[letterIndex].numDrinks - 8*num;
	y = yinit-1.0;
	tR = 1.0;
	tG = 1.0;
	tB = 1.0;
	if (remaining <= 8){
		for (j=l; j<remaining; j++){
			x = x1;
			s = 0.2;
			r = 1.0;
			tS = 1.0;
			tR = 1.0;
			tG = 1.0;
			tB = 1.0;
			t = possibleDrinks[j+start].name;
			for (a=0;a<5;a++){
				strcpy(nT[a], possibleDrinks[j+start].textIngredients[a]);
			}
			d = j+start;
			makeButton(j,x,y,z,s,r,g,b,tS,tR,tG,tB,t,nT,sel,d);
			y = y - 0.4 - (1+possibleDrinks[j+start].numlines)*0.2;
		}	
		numDrinksDisplayed = j;
		moreDrinks = -1;
	} else {
		for (j=l; j<8; j++){
			x = x1;
			s = 0.2;
			r = 1.0;
			tS = 1.0;
			tR = 1.0;
			tG = 1.0;
			tB = 1.0;
			t = possibleDrinks[j+start].name;
			for (a=0;a<5;a++){
				strcpy(nT[a], possibleDrinks[j+start].textIngredients[a]);
			}
			d = j+start;
			makeButton(j,x,y,z,s,r,g,b,tS,tR,tG,tB,t,nT,sel,d);
			y = y - 0.4 - (1+possibleDrinks[j+start].numlines)*0.2;
		}	
		numDrinksDisplayed = j;
		j++;
		x = x1;
		y = yinit;
		s = 0.2;
		r = 0.5;
		b = 1.0;
		tS = 1.0;
		t = "next 8";
		for (a=0;a<5;a++){
			strcpy(nT[a], "");
		}
		d = 0;
		makeButton(j,x,y,z,s,r,g,b,tS,tR,tG,tB,t,nT,sel,d);
		moreDrinks = j;
	}
	if (set != 0){
		j++;
		x = x1;
		y = yinit - 0.5;
		s = 0.2;
		r = 0.5;
		g = 0.0;
		b = 1.0;
		tS = 1.0;
		t = "previous 8";
		for (a=0;a<5;a++){
			strcpy(nT[a], "");
		}
		d = 0;
		makeButton(j,x,y,z,s,r,g,b,tS,tR,tG,tB,t,nT,sel,d);
		lastDrinks = j;
	} else {
		lastDrinks = -1;
	}
	if (numDrinks == 0){
		x = x1;
		if (dynamic == 1){
			y = yinit-0.5;
		} else {
			y = yinit+0.5;
		}
		s = 0.0;
		r = 0.0;
		g = 0.0;
		b = 1.0;
		tR = 1.0;
		tB = 1.0;
		tG = 1.0;
		tS = 1.5;
		t = "No drinks with";
		for (a=0;a<5;a++){
			strcpy(nT[a], "");
		}
		d = 0;
		makeButton(j,x,y,z,s,r,g,b,tS,tR,tG,tB,t,nT,sel,d);
		j++;
		y = y-0.5;
		t = "these ingredients";
		makeButton(j,x,y,z,s,r,g,b,tS,tR,tG,tB,t,nT,sel,d);
	} else {
		j++;
		x = x1;
		y = yinit+0.5;
		s = 0.0;
		r = 0.0;
		g = 0.0;
		b = 1.0;
		tS = 1.5;
		t = "Drinks Database";
		for (a=0;a<5;a++){
			strcpy(nT[a], "");
		}
		d = 0;
		makeButton(j,x,y,z,s,r,g,b,tS,tR,tG,tB,t,nT,sel,d);
	}
	j++;
	x = x1;
	y = yinit-7.3;
	s = 0.3;
	r = 0.0;
	g = 0.0;
	b = 0.5;
	tS = 1.0;
	t = "Cancel";
	for (a=0;a<5;a++){
		strcpy(nT[a], "");
	}
	d = 0;
	makeButton(j,x,y,z,s,r,g,b,tS,tR,tG,tB,t,nT,sel,d);
	back = j;
	j++;


	
	r = 0.5;

	k = 0;
	for (int i=0; i<26; i++){
		if (alphabetInfo[i].numDrinks != 0){
			if (k<13){
				x = x1 - 1.0;
				y = yinit - k*0.5;
			} else {
				x = x1 - 0.5;
				y = yinit - (k-13)*0.5;
			}

			k++;
			t = "";
			s = 0.2;
			makeButton(j,x,y,z,s,r,g,b,tS,tR,tG,tB,t,nT,sel,d);
			j++;
			s = 0.0;
			t = alphabet[i];
			x = x-0.1;
			makeButton(j,x,y,z,s,r,g,b,tS,tR,tG,tB,t,nT,sel,d);
			j++;
		}
	}
	numLetters = k;

	numButtons = j;
}


/*
 * void showDrinkChosen(int num)
 *
 * show the drink chosen
 */
void showDrinkChosen(int num)
{
	int a;
	char nT[5][60];	// the ingredients of the drink
	int j=0;
	double x = 0.0;		// x position		
	double y = 0.0;		// y position
	double z = 0.0;		// z position
	double s = 0.0;		// size of the ball (radius)
	double r = 0.0;		// amount of red of ball
	double g = 0.0;		// amount of green of ball
	double b = 0.0;		// amount of blue of ball
	double tS = 0.0;		// size of the ball (radius)
	double tR = 0.0;		// amount of red of ball
	double tG = 0.0;		// amount of green of ball
	double tB = 0.0;		// amount of blue of ball
	char * t = "";		// the name of the drink
	int sel = 0;		// if the ball is selected
	int d = 0;			// position in possible drinks array

	for (a=0; a<5; a++){
			strcpy(nT[a], "");
	}
	initBalls();
	t = drinks[num].name;
	tR = 1.0;
	tB = 0.5;
	tG = 0.0;
	tS = 1.5;
	x = x1;
	y = yinit;
	makeButton(j,x,y,z,s,r,g,b,tS,tR,tG,tB,t,nT,sel,d);
	j++;
	a=0;
	while(strcmp(drinks[num].ingredient[a], "null") != 0){
		t = drinks[num].textIngredientsAmounts[a];
		tB = 1.0;
		tG = 1.0;
		tS = 1.0;
		x = x1;
		y = yinit - (a+1)*0.3;
		makeButton(j,x,y,z,s,r,g,b,tS,tR,tG,tB,t,nT,sel,d);
		a++;
		j++;
	}
	t = "Edit Drink";
	b = 0.5;
	r = 0.5;
	s = 0.3;
	x = x1;
	y = yinit - (a+2)*0.3;
	makeButton(j,x,y,z,s,r,g,b,tS,tR,tG,tB,t,nT,sel,d);
	next2 = j;
	j++;
	t = "Delete Drink";
	b = 0.0;
	r = 1.0;
	s = 0.3;
	x = x1;
	y = yinit - (a+5)*0.3;
	makeButton(j,x,y,z,s,r,g,b,tS,tR,tG,tB,t,nT,sel,d);
	next = j;
	j++;

	x = x1;
	t = "Back to Drinks";
	r = 0.0;
	b = 0.5;
	y = yinit - 7.3;
	makeButton(j,x,y,z,s,r,g,b,tS,tR,tG,tB,t,nT,sel,d);
	back = j;
	j++;
	numButtons = j;
}

/*
 * void showDrinkChosen(int num)
 *
 * show the drink chosen
 */
void showPossibleDrinkChosen(int num)
{
	int a;
	char nT[5][60];	// the ingredients of the drink
	int j=0;
	double x = 0.0;		// x position		
	double y = 0.0;		// y position
	double z = 0.0;		// z position
	double s = 0.0;		// size of the ball (radius)
	double r = 0.0;		// amount of red of ball
	double g = 0.0;		// amount of green of ball
	double b = 0.0;		// amount of blue of ball
	double tS = 0.0;		// size of the ball (radius)
	double tR = 0.0;		// amount of red of ball
	double tG = 0.0;		// amount of green of ball
	double tB = 0.0;		// amount of blue of ball
	char * t = "";		// the name of the drink
	int sel = 0;		// if the ball is selected
	int d = 0;			// position in possible drinks array

	for (a=0; a<5; a++){
			strcpy(nT[a], "");
	}
	initBalls();
	t = possibleDrinks[num].name;
	tR = 1.0;
	tB = 0.5;
	tG = 0.0;
	tS = 1.5;
	x = x1;
	y = yinit;
	makeButton(j,x,y,z,s,r,g,b,tS,tR,tG,tB,t,nT,sel,d);
	j++;
	a=0;
	while(strcmp(possibleDrinks[num].ingredient[a], "null") != 0){
		t = possibleDrinks[num].textIngredientsAmounts[a];
		tB = 1.0;
		tG = 1.0;
		tS = 1.0;
		x = x1;
		y = yinit - (a+1)*0.3;
		makeButton(j,x,y,z,s,r,g,b,tS,tR,tG,tB,t,nT,sel,d);
		a++;
		j++;
	}

	s = 0.3;
	x = x1;
	t = "Back to Drinks";
	r = 0.0;
	b = 0.5;
	y = yinit - 7.3;
	makeButton(j,x,y,z,s,r,g,b,tS,tR,tG,tB,t,nT,sel,d);
	back = j;
	j++;
	numButtons = j;
}


int findPossibleDrinkSet(int letter){

	int index, set,i;

	index = 0;
	for (i=0; i<numDrinks; i++){
		if (_stricmp(drinks[i].name,alphabet[letter]) == -1){
			index = i+1;
		}
	}

	set = 0;
	while (index > 8){
		index = index - 8;
		set++;
	}
	return set;
}

void setChosenDrink (int index){
	newChosenDrink = drinks[index];
	confirmed = 0;
}

void setMYOChosenDrink (){
	newChosenDrink = MYODrink;
	confirmed = 0;
}

void setLastDrink (){
	drinkOrdered = 1;
	confirmed = 1;
	lastDrink = newChosenDrink;
}

void setMYOLastDrink (){
	drinkOrdered = 1;
	confirmed = 1;
	lastDrink = MYODrink;
}


void orderDrink(struct drink order){
	// set display to show the drink ordered
	// send message to machine
	// receive message from machine before continuing
	// update database of drinks and ingredients
		int a;
	char nT[5][60];	// the ingredients of the drink
	double x = 0.0;		// x position		
	double y = 0.0;		// y position
	double z = 0.0;		// z position
	double s = 0.0;		// size of the ball (radius)
	double r = 0.0;		// amount of red of ball
	double g = 0.0;		// amount of green of ball
	double b = 0.0;		// amount of blue of ball
	double tS = 0.0;		// size of the ball (radius)
	double tR = 0.0;		// amount of red of ball
	double tG = 0.0;		// amount of green of ball
	double tB = 0.0;		// amount of blue of ball
	char * t = "";		// the name of the drink
	int sel = 0;		// if the ball is selected
	int d = 0;			// position in possible drinks array
	int j=0;

	for (a=0; a<5; a++){
			strcpy(nT[a], "");
	}
	initBalls();

	t = "Order";
	s = 0.3;
	r = 0.5;
	b = 0.5;
	x = x3;
	y = yinit - 6.0;
	tR = 1.0;
	tG = 1.0;
	tB = 1.0;
	tS = 1.0;
	makeButton(j,x,y,z,s,r,g,b,tS,tR,tG,tB,t,nT,sel,d);
	next = j;
	j++;
	t = "Cancel";
	s = 0.3;
	r = 0.0;
	b = 0.5;
	x = x3;
	y = yinit - 7.0;
	tR = 1.0;
	tG = 1.0;
	tB = 1.0;
	tS = 1.0;
	makeButton(j,x,y,z,s,r,g,b,tS,tR,tG,tB,t,nT,sel,d);
	back = j;
	j++;
	numButtons = j;

}

int findLetter(int letterNum){
	int i,count,letter;
	letter = 0;
	count = 0;
	for (i=0; i<26; i++){
		if (alphabetInfo[i].numDrinks != 0){
			if (count == letterNum){
				letter = i;;
			}
			count++;
		}
	}
	return letter;
}

void createOrderEdit(){
	int a;
	char nT[5][60];	// the ingredients of the drink
	double x = 0.0;		// x position		
	double y = 0.0;		// y position
	double z = 0.0;		// z position
	double s = 0.0;		// size of the ball (radius)
	double r = 0.0;		// amount of red of ball
	double g = 0.0;		// amount of green of ball
	double b = 0.0;		// amount of blue of ball
	double tS = 0.0;		// size of the ball (radius)
	double tR = 0.0;		// amount of red of ball
	double tG = 0.0;		// amount of green of ball
	double tB = 0.0;		// amount of blue of ball
	char * t = "";		// the name of the drink
	int sel = 0;		// if the ball is selected
	int d = 0;			// position in possible drinks array
	int j=0;

	for (a=0; a<5; a++){
			strcpy(nT[a], "");
	}

	j = numButtons;

	t = "Order";
	s = 0.3;
	r = 0.5;
	b = 0.5;
	x = xTrans - dynSize + 1.0;
	y = yTrans + dynSize + 1.0;
	tR = 1.0;
	tG = 1.0;
	tB = 1.0;
	tS = 1.0;
	makeButton(j,x,y,z,s,r,g,b,tS,tR,tG,tB,t,nT,sel,d);
	selectMiddleDrink = j;
	j++;
	t = "Edit";
	s = 0.3;
	r = 1.0;
	b = 0.2;
	x = xTrans + 0.5;
	y = yTrans + dynSize + 1.0;
	tR = 1.0;
	tG = 1.0;
	tB = 1.0;
	tS = 1.0;
	makeButton(j,x,y,z,s,r,g,b,tS,tR,tG,tB,t,nT,sel,d);
	editMiddleDrink = j;
	j++;
	t = "Cancel";
	s = 0.3;
	r = 0.0;
	b = 0.5;
	x = xTrans + 2.5;
	y = yTrans + dynSize + 1.0;
	tR = 1.0;
	tG = 1.0;
	tB = 1.0;
	tS = 1.0;
	makeButton(j,x,y,z,s,r,g,b,tS,tR,tG,tB,t,nT,sel,d);
	cancelMiddleDrink = j;
	j++;
	numButtons = j;
}

void resetMiddleDrink(){
	selectMiddleDrink = -1;
	editMiddleDrink = -1;
	cancelMiddleDrink = -1;
}

void removeOrderEdit(){
	resetMiddleDrink();
	numButtons = numButtons - 3;
}

void setRandomDrinkCenter(){

	int number;

	num = rand();
	num = (num / 32767) * numPossibleDrinks;

	number = (int)num;

	if (dynamic == 1){
		setVisibleDrinks(-1,number);
	}
	setChosenDrink(number);
	

	dynButton = 1;
	
	if (dynLastButton == 0){
		createOrderEdit();
	}
	randomDrink = 1;
}

void createOrder(){

	int j;
	int a;
	char nT[5][60];	// the ingredients of the drink
	double x = x1;		// x position		
	double y = 0.0;		// y position
	double z = 0.0;		// z position
	double s = 0.3;		// size of the ball (radius)
	double r = 0.5;		// amount of red of ball
	double g = 0.0;		// amount of green of ball
	double b = 0.5;		// amount of blue of ball
	double tS = 1.0;		// size of the ball (radius)
	double tR = 1.0;		// amount of red of ball
	double tG = 1.0;		// amount of green of ball
	double tB = 1.0;		// amount of blue of ball
	char * t = "";		// the name of the drink
	int sel = 0;		// if the ball is selected
	int d = 0;			// position in possible drinks array

	for (a=0; a<5; a++){
			strcpy(nT[a], "");
	}

	j = numButtons;
	if (MYODrink.numIngredients != 0){
		t = "Order Drink";
		if (MYOindex < 10){
			y = yinit - (4+MYODrink.numIngredients)*0.5;
		} else {
			y = yinit - 6.5;
		}
		makeButton(j,x,y,z,s,r,g,b,tS,tR,tG,tB,t,nT,sel,d);
		next = j;
		j++;
	} else {
		next = -1;
	}
	numButtons = j;

}

void createAlphabet(){
	int j;
	int a;
	int k;
	char nT[5][60];	// the ingredients of the drink
	double x = 0.0;		// x position		
	double y = 0.0;		// y position
	double z = 0.0;		// z position
	double s = 0.0;		// size of the ball (radius)
	double r = 0.0;		// amount of red of ball
	double g = 0.0;		// amount of green of ball
	double b = 0.0;		// amount of blue of ball
	double tS = 0.0;		// size of the ball (radius)
	double tR = 0.0;		// amount of red of ball
	double tG = 0.0;		// amount of green of ball
	double tB = 0.0;		// amount of blue of ball
	char * t = "";		// the name of the drink
	int sel = 0;		// if the ball is selected
	int d = 0;			// position in possible drinks array

	for (a=0; a<5; a++){
			strcpy(nT[a], "");
	}
	j = numButtons;
	
	s = 0.3;
	r = 0.5;
	g = 0.0;
	b = 0.5;
	tS = 1.0;
	tR = 1.0;
	tG = 1.0;
	tB = 1.0;

	for (a=0;a<5;a++){
		strcpy(nT[a], "");
	}
	d = 0;
	
	if (dynamic == 1){
		x = x4;
		y = yinit+0.5;
	} else {
		x = x5;
		y = yinit;
	}
	k = 0;
	for (int i=0; i<26; i++){
		if (alphabetInfo[i].numDrinks != 0){
			if (dynamic == 1){
				if (k<13){
					x = x4 + (k-1)*0.5;
					y = yinit + 0.5;
				} else {
					x = x4 + (k-14)*0.5;
					y = yinit;
				}
			} else {
				if (k<13){
					x = x5;
					y = yinit - k*0.5;
				} else {
					x = x5 + 0.5;
					y = yinit - (k-13)*0.5;
				}
			}
			k++;
			t = "";
			s = 0.2;
			makeButton(j,x,y,z,s,r,g,b,tS,tR,tG,tB,t,nT,sel,d);
			j++;
			s = 0.0;
			t = alphabet[i];
			x = x-0.1;
			makeButton(j,x,y,z,s,r,g,b,tS,tR,tG,tB,t,nT,sel,d);
			j++;
		}
	}
	numLetters = k;

	numButtons = j;
	
}

void removePipeIngredient(int pipe){

	int i;
	int check = 0;
	for (i=0; i<numPipeIngredients; i++){
		if (check == 1){
			pipeIngredients[i-1] = pipeIngredients[i];
		}
		if (pipeIngredients[i].pipeNum == pipe){
			check = 1;
		}
	}
	if(check == 1){
		numPipeIngredients = numPipeIngredients-1;
		strcpy(pipeIngredients[numPipeIngredients].name,"null");
		pipeIngredients[numPipeIngredients].pipeAmount = 0;
		strcpy(pipeIngredients[numPipeIngredients].pipeAmountText,"");
		pipeIngredients[numPipeIngredients].pipeNum = 0;
	}
}

void setPipeAmount(int pipe, int pipeAmount){
	int i;
	int decimal, sign;
	for(i = 0; i<numPipeIngredients; i++){
		if (pipeIngredients[i].pipeNum == pipe){
			pipeIngredients[i].pipeAmount = pipeAmount;
			strcpy(pipeIngredients[i].pipeAmountText,_fcvt((pipeIngredients[i].pipeAmount),0,&decimal,&sign));
		}
	}
}

void setNewPipeAmount(int pipe, int pipeAmount){
	int decimal, sign;
	newPipeIngredient.pipeAmount = pipeAmount;
	strcpy(newPipeIngredient.pipeAmountText,_fcvt((newPipeIngredient.pipeAmount),0,&decimal,&sign));
	pipeIngredients[numPipeIngredients] = newPipeIngredient;
	numPipeIngredients = numPipeIngredients + 1;
	strcpy(pipeIngredients[numPipeIngredients].name,"null");
}

void removePossibleIngredient(int ingredient){
	int i;
	for(i = ingredient; i<numingredients; i++){
		strcpy(ingredients[i],ingredients[i+1]);
	}
	numingredients = numingredients - 1;
	strcpy(ingredients[numingredients],"null");
}

void removeDrink(int drinkIndex){
	int i;
	for(i=drinkIndex; i<numDrinks; i++){
		drinks[i] = drinks[i+1];
	}
	numDrinks = numDrinks - 1;
	strcpy(drinks[numDrinks].name,"null");
	writeDrinks();
	initDrinks();
	setAlphabetDrinks();
}

/*
 * void action(int ball)
 *
 * take an action depending on the state and the input
 */
void action(int ball)
{
	int number,number1,i;
	switch (state){
	case INTRO:
		if (ball == next){
			setAlphabetDrinks();
			choicesBalls();
			state = CHOICES;
		}
		break;
	case CHOICES:
		if (ball == next){
			ingredientsSet = 0;
			clearSelected();
			setPossibleDrinks(1);
			drawIngredients(ingredientsSet);
			state = INGREDIENTS;
		} else if (ball == next2){
			set = 0;
			setAlphabetDrinks();
			showDrinks(set,0);
			chosenDrink = 0;
			state = ALLDRINKS;
		} else if (ball == next3){
			// display all of the possible ingredients - all that are in the drinks?
			ingredientsSet = 0;
			clearSelected();
			drawPossibleIngredients(1);
			state = POSSIBLEINGREDIENTS;
		} else if (ball == back){
			openingBalls();
			state = INTRO;
		} else if (ball == next4){	// random drink
		}
		break;
	case INGREDIENTS:
		if (ball == back){	// Cancel
			choicesBalls();
			state = CHOICES;
		} else if (ball == next){	// View Drinks
			set = 0;
			setPossibleDrinks(1);
			setPossibleAlphabetDrinks();
			showPossibleDrinks(set,0);
			chosenDrink = 0;
			state = DRINKS;
		} else if ((ball >= MAXPIPES) && (ball < 2*MAXPIPES)){
			// set pipe amount
			pipeToSet = (ball - MAXPIPES+1);
			for (i=0; i<MAXPIPES; i++){
				if (pipeIngredients[i].pipeNum == pipeToSet){
					strcpy(newPipeIngredient.name,pipeIngredients[i].name);
				}
			}
			selectAmount(ball - MAXPIPES+1);
			state = SELECTAMOUNT;
		} else if ((ball >= 3*MAXPIPES) && (ball < 4*MAXPIPES)){
			// add or remove ingredients
			if (emptyPipe[ball-3*MAXPIPES] == 0){
				// add ingredient
				// select from possible ingredients
				newPipeIngredient.pipeNum = (ball-3*MAXPIPES+1);
				ingredientsSet = 0;
				clearSelected();
				writePipeAmounts();
				drawPossibleIngredients(0);
				numButtons = numButtons - 4;
				state = SELECTINGREDIENT;
			} else {
				// remove ingredient
				removePipeIngredient(ball-3*MAXPIPES+1);
				clearSelected();
				writePipeAmounts();
				setPossibleDrinks(1);
				drawIngredients(ingredientsSet);
			}
		}
		break;
	case POSSIBLEINGREDIENTS:
		if (ball == back){	// Cancel
			choicesBalls();
			state = CHOICES;
		} else if (ball == next){	// View Drinks
			set = 0;
			setPossibleDrinks(0);
			setPossibleAlphabetDrinks();
			showPossibleDrinks(set,0);
			chosenDrink = 0;
			state = DRINKS;
		} else if (ball == next2){	// Add another ingredient
			strcpy(newIngredient,"");
			enterName(0);
			state = NEWINGREDIENT;
		}  else if (ball == moreDrinks){
			ingredientsSet = ingredientsSet+1;
			drawPossibleIngredients(1);
		} else if (ball == lastDrinks){
			ingredientsSet = ingredientsSet-1;
			drawPossibleIngredients(1);
		} else if ((ball < numIngredientsDisplayed) && (ball >= 0)){
			if (buttons[ball].selected == 0){
				buttons[ball].red = 0.0;
				buttons[ball].blue = 1.0;
				buttons[ball].selected = 1;
				selected[(20*ingredientsSet + ball)] = 1;
			} else {
				buttons[ball].red = 1.0;
				buttons[ball].blue = 0.0;
				buttons[ball].selected = 0;
				selected[(20*ingredientsSet + ball)] = 0;
			}
		} else if ((ball >= removeStart) && (ball <= removeStart + numIngredientsDisplayed)){
			removePossibleIngredient(20*ingredientsSet + ball-removeStart);
			writeIngredients();
			initIngredients();
			drawPossibleIngredients(1);
		}
		break;
	case SELECTINGREDIENT:
		if(ball == back){
			ingredientsSet = 0;
			clearSelected();
			setPossibleDrinks(1);
			drawIngredients(ingredientsSet);
			state = INGREDIENTS;			
		} else if (ball == moreDrinks){
			ingredientsSet = ingredientsSet+1;
			drawPossibleIngredients(0);
		} else if (ball == lastDrinks){
			ingredientsSet = ingredientsSet-1;
			drawPossibleIngredients(0);
		} else if ((ball < numIngredientsDisplayed) && (ball >= 0)){
			strcpy(newPipeIngredient.name,ingredients[20*ingredientsSet+ball]);
			selectAmount(newPipeIngredient.pipeNum);
			state = SELECTNEWAMOUNT;
		}
		break;
	case DRINKS:
		 if (ball == back){
 			setAllPossibleDrinks();
			setAlphabetDrinks();
			clearSelected();
			ingredientsSet = 0;
			choicesBalls();
			state = CHOICES;
		} else if (ball == moreDrinks){
			set = set+1;
			showPossibleDrinks(set,0);
		} else if (ball == lastDrinks){
			set = set-1;
			showPossibleDrinks(set,0);
		} else if (ball == next){
		} else if ((ball < numDrinksDisplayed) && (ball >= 0)){
			chosenDrink = ball;
			setChosenDrink(buttons[chosenDrink].drink);
			editedDrinkIndex = buttons[chosenDrink].drink;
			showPossibleDrinkChosen(buttons[chosenDrink].drink);
			state = DRINKCHOSEN;
		} else if ((ball < numButtons-1) && (ball >= 0)){
			number1 = numLetters - (((numButtons) - ball)/2);
			number = findLetter(number1);
			letter = number;
			letterSet = 0;
			set = 0;
			showPossibleLetterDrinks(number,set);
			state = LETTERS;
		}
		break;
	case ALLDRINKS:
		 if (ball == back){
 			setAllPossibleDrinks();
			setAlphabetDrinks();
			choicesBalls();
			state = CHOICES;
		 } else if (ball == moreDrinks){
			set = set+1;
			showDrinks(set,0);
		} else if (ball == lastDrinks){
			set = set-1;
			showDrinks(set,0);
		} else if (ball == next){
			initMYODrink();
			strcpy(MYODrink.name,"");
			enterName(1);
			state = NEWDRINK;
		} else if ((ball < numDrinksDisplayed) && ball >= 0){
			chosenDrink = ball;
			setChosenDrink(buttons[chosenDrink].drink);
			editedDrinkIndex = buttons[chosenDrink].drink;
			showDrinkChosen(buttons[chosenDrink].drink);
			state = ALLDRINKCHOSEN;
		} else if ((ball < (numButtons)) && (ball >= 0)){
			number1 = numLetters - (((numButtons) - ball)/2);
			number = findLetter(number1);
			letter = number;
			set = 0;
			showLetterDrinks(number,0);
			state = ALLLETTERS;
		}
		break;
	case ALLDRINKCHOSEN:
		if (ball == back){
			showDrinks(set,0);
			chosenDrink = 0;
			state = ALLDRINKS;
		} else if (ball == next2){ // edit drink
			newDrink = 0;
			MYOset = 0;
			drawMYODrink(-3);
			state = MYODRINK;
		} else if (ball == next){	// remove drink
			removeDrink(editedDrinkIndex);
			showDrinks(set,0);
			chosenDrink = 0;
			state = ALLDRINKS;
		}
		break;
	case DRINKCHOSEN:
		if (ball == back){
			showPossibleDrinks(set,0);
			chosenDrink = 0;
			state = DRINKS;
		}  else if (ball == next2){ // edit drink
			newDrink = 0;
			MYOset = 0;
			drawMYODrink(-3);
			state = MYODRINK;
		} else if (ball == next){	// remove drink
			removeDrink(editedDrinkIndex);
			showPossibleDrinks(set,0);
			chosenDrink = 0;
			state = DRINKS;
		}
		break;
	case MYOINGREDIENT:
		if (ball == back){
			choicesBalls();
			state = CHOICES;
		} else if (ball == back2){	// cancel this ingredient
			MYOset = 0;
			drawMYODrink(-1);
			state = MYODRINK;
		}  else if (ball == moreDrinks){
			ingredientsSet = ingredientsSet+1;
			drawMYOIngredients(ingredientsSet);
		} else if (ball == lastDrinks){
			ingredientsSet = ingredientsSet-1;
			drawMYOIngredients(ingredientsSet);
		} else if ((ball < numIngredientsDisplayed) && (ball >= 0)){
			drawMYOAmounts(20 * ingredientsSet + ball);
			state = MYOAMOUNT;
		} 
		break;
	case MYOAMOUNT:
		if (ball == back){
			choicesBalls();
			state = CHOICES;
		} else if (ball == back2){	// cancel this ingredient
			MYOset = 0;
			drawMYODrink(-1);
			setAlphabetDrinks();
			state = MYODRINK;
		}  else if ((ball < MAXAMOUNTS) && (ball >= 0)){
			MYOset = 0;
			drawMYODrink(ball);
			setAlphabetDrinks();
			state = MYODRINK;
		} 
		break;
	case MYODRINK:
		if (ball == back){
			setAllPossibleDrinks();
			setAlphabetDrinks();
			choicesBalls();
			state = CHOICES;
		} else if (ball == next){	// save changes
			if (newDrink == 0){
				drinks[editedDrinkIndex] = MYODrink;
			} else {
				drinks[numDrinks] = MYODrink;
				numDrinks = numDrinks+1;
				strcpy(drinks[numDrinks].name,"null");
			}
			writeDrinks();
			initDrinks();
			setAllPossibleDrinks();
			setAlphabetDrinks();
			choicesBalls();
			state = CHOICES;
		} else if (ball == next2){	// add another ingredient
			ingredientsSet = 0;
			drawMYOIngredients(ingredientsSet);
			state = MYOINGREDIENT;
		} else if (ball == lastDrinks){
			MYOset = MYOset - 1;
			drawMYODrink(-2);
		} else if (ball == moreDrinks){
			MYOset = MYOset + 1;
			drawMYODrink(-2);
		} else if ((ball <= numIngredientsDisplayed) && (ball >= 0)){
			editIngredientIndex = ball + MYOset * 10;
			drawEditMYOIngredient();
			state = EDITINGREDIENT;
		}
		break;
	case EDITINGREDIENT:
		if (ball == next){			// remove ingredient
			removeMYOIngredient(editIngredientIndex);
			MYOset = 0;
			drawMYODrink(-2);
			state = MYODRINK;
		} else if (ball == back){	// cancel edit
			MYOset = 0;
			drawMYODrink(-2);
			state = MYODRINK;
		} else if ((ball > addAmountStart-1) && (ball < addAmountStop)){
			increaseMYOIngredient(editIngredientIndex,MYOAmounts[ball - addAmountStart]);
			MYOset = 0;
			drawMYODrink(-2);
			state = MYODRINK;
		} else if ((ball > removeAmountStart-1)&&(ball < removeAmountStop)){
			decreaseMYOIngredient(editIngredientIndex, MYOAmounts[ball - removeAmountStart]);
			MYOset = 0;
			drawMYODrink(-2);
			state = MYODRINK;
		} 
		break;
	case LETTERS:
		if (ball == back){
			set = 0;
 			showPossibleDrinks(set,0);
			state = DRINKS;
		} else if (ball == moreDrinks){
			set = set+1;
			showPossibleLetterDrinks(letter, set);
		} else if (ball == lastDrinks){
			set = set-1;
			showPossibleLetterDrinks(letter, set);
		} else if ((ball < numDrinksDisplayed) && (ball >= 0)){
			chosenDrink = ball;
			setChosenDrink(buttons[chosenDrink].drink);
			editedDrinkIndex = buttons[chosenDrink].drink;
			showPossibleDrinkChosen(buttons[chosenDrink].drink);
			state = DRINKCHOSEN;
		} else if ((ball < numButtons-1) && (ball >= 0)){
			number1 = numLetters - (((numButtons) - ball)/2);
			number = findLetter(number1);
			letter = number;
			set = 0;
			showPossibleLetterDrinks(number,set);
			state = LETTERS;
		}
		break;
	case ALLLETTERS:
		 if (ball == back){
			 set = 0;
			showDrinks(set,0);
			state = ALLDRINKS;
		 } else if (ball == moreDrinks){
			set = set+1;
			showLetterDrinks(letter, set);
		} else if (ball == lastDrinks){
			set = set-1;
			showLetterDrinks(letter, set);
		} else if ((ball < numDrinksDisplayed) && (ball >= 0)){
			chosenDrink = ball;
			setChosenDrink(buttons[chosenDrink].drink);
			editedDrinkIndex = buttons[chosenDrink].drink;
			showDrinkChosen(buttons[chosenDrink].drink);
			state = ALLDRINKCHOSEN;
		} else if ((ball < (numButtons-1)) && (ball >= 0)){
			number1 = numLetters - (((numButtons) - ball)/2);
			number = findLetter(number1);
			letter = number;
			set = 0;
			showLetterDrinks(number,0);
			state = ALLLETTERS;
		}
		break;
	case NEWINGREDIENT:
		if (ball == back){
			ingredientsSet = 0;
			numingredients = countNumIngredients();
			numPipeIngredients = countNumPipeIngredients();
 			drawPossibleIngredients(1);
			state = POSSIBLEINGREDIENTS;
		} else if (ball == next){
			numingredients = countNumIngredients();
			strcpy(ingredients[numingredients],newIngredient);
			numingredients = numingredients+1;
			strcpy(ingredients[numingredients],"null");
			numingredients = countNumIngredients();
			numPipeIngredients = countNumPipeIngredients();
			writeIngredients();
			initIngredients();
			ingredientsSet = 0;
			clearSelected();
			numingredients = countNumIngredients();
			numPipeIngredients = countNumPipeIngredients();
			drawPossibleIngredients(1);
			state = POSSIBLEINGREDIENTS;
		} else if ((ball < 27*2) && (ball >=0)){
			letter = ball / 2;
			strcat(newIngredient,alphabet[letter]);
			_strlwr(newIngredient);
			enterName(0);
		}
		break;
	case NEWDRINK:
		if (ball == back){	// back to drinks
			set = 0;
			numingredients = countNumIngredients();
			numPipeIngredients = countNumPipeIngredients();
			setAlphabetDrinks();
			showDrinks(set,0);
			chosenDrink = 0;
			state = ALLDRINKS;
		} else if (ball == next){	//save the drink name, start to make drink
			clearSelected();
			ingredientsSet = 0;
			newDrink = 1;
			numingredients = countNumIngredients();
			numPipeIngredients = countNumPipeIngredients();
			drawMYOIngredients(ingredientsSet);
			state = MYOINGREDIENT;
		} else if ((ball <27*2) && (ball >=0)){
			letter = ball / 2;
			strcat(MYODrink.name,alphabet[letter]);
			_strlwr(MYODrink.name);
			enterName(1);
		}
		break;
	case SELECTAMOUNT:
		if (ball == back){
			ingredientsSet = 0;
			numingredients = countNumIngredients();
			numPipeIngredients = countNumPipeIngredients();
			clearSelected();
			setPossibleDrinks(1);
			drawIngredients(ingredientsSet);
			state = INGREDIENTS;	
		} else if ((ball >= 0) && (ball < 10)){
			numingredients = countNumIngredients();
			numPipeIngredients = countNumPipeIngredients();
			setPipeAmount(pipeToSet,100*(10-ball));
			writePipeAmounts();
			clearSelected();
			setPossibleDrinks(1);
			drawIngredients(ingredientsSet);
			state = INGREDIENTS;
		} else if ((ball >= 10) && (ball <20)){
			numingredients = countNumIngredients();
			numPipeIngredients = countNumPipeIngredients();
			setPipeAmount(pipeToSet,100*(20-ball)-50);
			writePipeAmounts();
			clearSelected();
			setPossibleDrinks(1);
			drawIngredients(ingredientsSet);
			state = INGREDIENTS;
		}
		break;
	case SELECTNEWAMOUNT:
		if (ball == back){
			ingredientsSet = 0;
			numingredients = countNumIngredients();
			numPipeIngredients = countNumPipeIngredients();
			clearSelected();
			drawIngredients(ingredientsSet);
			state = INGREDIENTS;	
		} else if ((ball >= 0) && (ball < 10)){
			numingredients = countNumIngredients();
			numPipeIngredients = countNumPipeIngredients();
			setNewPipeAmount(newPipeIngredient.pipeNum,100*(10-ball));
			writePipeAmounts();
			clearSelected();
			setPossibleDrinks(1);
			drawIngredients(ingredientsSet);
			state = INGREDIENTS;
			
		} else if ((ball >= 10) && (ball <20)){
			numingredients = countNumIngredients();
			numPipeIngredients = countNumPipeIngredients();
			setNewPipeAmount(newPipeIngredient.pipeNum,100*(20-ball)-50);
			writePipeAmounts();
			clearSelected();
			setPossibleDrinks(1);
			drawIngredients(ingredientsSet);
			state = INGREDIENTS;
		}
	}
}


/*
 * void mouse(int button, int state, int x, int y)
 *
 * determine which button was pressed, and take an action
 */
void mouse(int button, int state, int x, int y) 
{
	int b,c,d;
	double xb,yb;

	d = 0;
	c = 0;
	if (state == GLUT_DOWN){
    switch (button) {
      case GLUT_LEFT_BUTTON:
		  // determine the mouse position in the matrix of the screen
		  if (width <= (height * 2)){
			  mouseposx = (((double)x * 12.0) / (double)width) - 6.0;
			  mouseposy = (((((double)y * 6.0) / (double)height) - 3.0) * (-1)) * ((2 * (double)height) / (double)width);
		  } else {
			  mouseposx = ((((double)x * 12.0) / (double)width) - 6.0) * ((double)width / (2 * (double)height));
			  mouseposy = ((((double)y * 6.0) / (double)height) - 3.0) * (-1);
		  }
		  // check whether the mouse was clicked while over one of the balls
		  for (b = 0; b < numButtons; b++) {
			  if (((buttons[b].posx - buttons[b].size) <= mouseposx) 
				  && ((buttons[b].posx + buttons[b].size) >= mouseposx)){
				  if (((buttons[b].posy - buttons[b].size) <= mouseposy)
					  && ((buttons[b].posy + buttons[b].size) >= mouseposy)){
					  c=b;
					  d = 1;

				  }
			  }
		  }
		  if (d == 1){
			  if (dynButton == 0){
				  dynLastButton = 0;
			  } else {
				  dynLastButton = 1;
			  }
			  dynButton = 0;
			  action(c);
			  display();
			  if (randomDrink == 0){
				  resetMiddleDrink();
			  } else {
				  randomDrink = 0;
			  }

		  }
		  if (dynamic == 1){
		  for (b = 0; b < numVisibleDrinks; b++) {
			  if (movingButtons[b].posx <= -dynSize){
				  xb = -dynSize;
			  } else if (movingButtons[b].posx >= dynSize){
				  xb = dynSize;
			  } else {
				  xb = movingButtons[b].posx;
			  }
			  if (movingButtons[b].posy <= -dynSize){
				  yb = -dynSize;
			  } else if (movingButtons[b].posy >= dynSize){
				  yb = dynSize;
			  } else {
				  yb = movingButtons[b].posy;
			  }
  			  if ((((xb - movingButtons[b].size + xTrans)) <= mouseposx) 					
				  && (((xb + movingButtons[b].size + xTrans)) >= mouseposx)){
				  if ((((yb - movingButtons[b].size + yTrans)) <= mouseposy)
					  && (((yb + movingButtons[b].size + yTrans)) >= mouseposy))
				  {
					  if (dynButton == 0){
						dynLastButton = 0;
					  } else {
						dynLastButton = 1;
					  }
					  dynButton = 1;
					  action(-5);
					  if (dynLastButton == 0){
						  createOrderEdit();
					  }
					  if (b != centerballBalls){
						  setVisibleDrinks(b,0);
					  }
				  }
			  }
		  }
		  }
		  break;
	  case GLUT_MIDDLE_BUTTON:
		  break;
      case GLUT_RIGHT_BUTTON:
		  exit(0);
         break;
      default:
         break;
   }
	}
}

/*
 * void passiveMotion(int x, int y)
 *
 * detects when the mouse is over one of the dynamic buttons, and displays
 * the ingredients of that drink at that point in time
 */
void passiveMotion(int x, int y)
{
	int show,b;
	double xb,yb;

	// determine the mouse position in the matrix of the screen
	if (width <= (height * 2)){
		  mouseposx = (((double)x * 12.0) / (double)width) - 6.0;
		  mouseposy = (((((double)y * 6.0) / (double)height) - 3.0) * (-1)) * ((2 * (double)height) / (double)width);
	} else {
		mouseposx = ((((double)x * 12.0) / (double)width) - 6.0) * ((double)width / (2 * (double)height));
		  mouseposy = ((((double)y * 6.0) / (double)height) - 3.0) * (-1);
	}
	show = -1;
	// check whether the mouse is over one of the balls
	// if it is, display the ingredients of that drink
	for (b = 0; b < numVisibleDrinks; b++) {
		if (movingButtons[b].posx <= -dynSize){
			xb = -dynSize;
		} else if (movingButtons[b].posx >= dynSize){
			xb = dynSize;
		} else {
			xb = movingButtons[b].posx;
		}
		if (movingButtons[b].posy <= -dynSize){
			yb = -dynSize;
		} else if (movingButtons[b].posy >= dynSize){
			yb = dynSize;
		} else {
			yb = movingButtons[b].posy;
		}
		if ((((xb - movingButtons[b].size + xTrans)) <= mouseposx) 					
			&& (((xb + movingButtons[b].size + xTrans)) >= mouseposx)){
			if ((((yb - movingButtons[b].size + yTrans)) <= mouseposy)
				&& (((yb + movingButtons[b].size + yTrans)) >= mouseposy))
			{
				show = b;
			}
		}
	}
	showIngredients = show;
}

/*
 * keyboard(unsigned char key, int x, int y)
 *
 * if the excape key is pressed, exit
 */
void keyboard(unsigned char key, int x, int y)
{
   switch (key) {
      case 27:
		  //writePipeAmounts();
		  //writeIngredients();
		  //writeDrinks();
         exit(0);
         break;
	  case 32:
		  action(-3);
		  display();
		  break;
   }
}


/*
 * void reshape (int w, int h)
 * 
 * when the window is reshaped by the user, reshape the graphics
 * makes sure that the graphics is not distorted
 */
void reshape(int w, int h)
{
	width = w;
	height = h;
   	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	if (w <= (h * 2))
		glOrtho (-6.0, 6.0, -3.0*((GLdouble)h*2)/(GLdouble)w, 
		3.0*((GLdouble)h*2)/(GLdouble)w, -10.0, 10.0);
	else
		glOrtho (-6.0*(GLdouble)w/((GLdouble)h*2), 
		6.0*(GLdouble)w/((GLdouble)h*2), -3.0, 3.0, -10.0, 10.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}


/*
 * int main(int argc, char** argv)
 *
 * initialise and start main loop
 */
int main(int argc, char** argv)
{
	initIngredients();
	initPipeAmounts();
	initDrinks();
	setAllPossibleDrinks();
	openingBalls();
	// set the random seed
	srand( (unsigned)time( NULL ) );
	// set up the display
	glutInit(&argc, argv);
	glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutCreateWindow(argv[0]);
	glutFullScreen();
	init();
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc (keyboard);
	glutMouseFunc (mouse);
	if (dynamic == 1){
	glutPassiveMotionFunc(passiveMotion);
	glutIdleFunc(moveBalls);
	}
	glutMainLoop(); 
	return(0);
}