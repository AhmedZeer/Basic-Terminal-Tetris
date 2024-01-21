#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <unistd.h>
#include <conio.h>
#include <windows.h>

#define TRUE 1
#define FALSE 0
#define PARCALAR 6 

char GameOn = TRUE;
int skor = 0;

void cursorSet( int x, int y)
{
    COORD coord;
    coord.X = x;
    coord.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

/*This Struct is used to save information about the tetrimino*/
typedef struct {
    char **boyut;
    int en;        
    int boy;      
    int x;          
    int y;         
    int tmp_en;
} Parca;

Parca eldeParca;

const Parca parcalar[PARCALAR] = {
    {(char *[]){(char[]){1,1,0},(char[]){1,1,0}}, 2, 2},                       // kare sekli en 2 boyu 2
    {(char *[]){(char[]){1,0,0}}, 1, 1},                                      // nokta sekli en 1 boyu 1
    {(char *[]){(char[]){1,1,0},(char[]){0,1,1},(char[]){0,0,0}}, 3, 2},     // Z sekli en 3 boyu 2
    {(char *[]){(char[]){1,1,1},(char[]){0,1,0},(char[]){0,0,0}}, 3, 2},    // T sekli en 3 boyu 2
    {(char *[]){(char[]){1,1,1},(char[]){0,0,0},(char[]){0,0,0}}, 3, 1},   // cubuk sekli en 3 boyu 1
    {(char *[]){(char[]){1,1,1},(char[]){1,0,0},(char[]){0,0,0}}, 3, 2},  // L sekli en 3 boyu 2
};

char getChar()
{
	return _getch();    //kullanicinin surekli Enter'a basmamasi icin fonk.
}


/* girdi olarak verilen parcanin boyutunu kopya adli degiskende saklariz
ardindan sutun kadar yer acariz ve son olarak her bir karakter sutunu icin yer de acariz*/
Parca parcaKopya( Parca parca )
{
	Parca new_shape = parca;
	char **copyshape = parca.boyut;
	new_shape.boyut = (char**)malloc(new_shape.en*sizeof(char*));
    int i, j;
    for(i = 0; i < new_shape.en; i++){
		new_shape.boyut[i] = (char*)malloc(new_shape.en*sizeof(char));
		for(j=0; j < new_shape.en; j++) {
			new_shape.boyut[i][j] = copyshape[i][j];
		}
    }
    return new_shape;
}


/* kopyalama isleminin tam tersi ilk once karakter basine dusen yeri free ediyoruz
sonra da butun boyutunkini free ediyoruz.*/
void parcaSil( Parca parca )            
{
    int i;
    for(i = 0; i < parca.en; i++){
		free(parca.boyut[i]);
    }
    free(parca.boyut);
}

/* parca hareket ettirmek istedigimiz zaman onun 'oyun' sinirlarini assmasini VE diger parcalarin
ustune binmesini istemiyoruz dolasıyla boyle bir kontrol fonksiyonu yapabilirz.*/
int koordKontrol( Parca parca, int r, int c, char oyun[r][c])
{   
	char **array = parca.boyut;
	int i, j;
	for( i=0 ; i<parca.en ; i++) {
		for( j=0 ; j<parca.en ; j++){
			if( (parca.y+j < 0 || parca.y+j >= c || parca.x+i >= r)){ //Out of borders
				if(array[i][j]) //but is it just a phantom?
					return FALSE;
			}
			else if(oyun[parca.x+i][parca.y+j] && array[i][j])
				return FALSE;
		}
	}
	return TRUE;
}

void parcaGetir( int r, int c, char oyun[r][c] )        // rastgele parca alma ve onu rastgele bir yere atama.
{
	Parca new_shape = parcaKopya(parcalar[rand()%PARCALAR]);

    new_shape.y = rand()%(c-new_shape.en+1);
    new_shape.x = new_shape.boy-1;
    parcaSil(eldeParca);
	eldeParca = new_shape;
    eldeParca.tmp_en = eldeParca.en;
	if(!koordKontrol(eldeParca, r, c, oyun)){
		GameOn = FALSE;
	}                           // yeni gelen parca uste binerse oyunun bitmesi icin. 
}

void parcaDondur1( Parca parca )                    //saat yonunde dondur. /*!!!!!!!!!!!!!!!!!!!*/
{
	Parca temp = parcaKopya(parca);
	int i, j, k, width, height;
	width = parca.en;
    height = parca.boy;
    parca.en = temp.boy;
    parca.boy = temp.en;
	for(i = 0; i < width ; i++){
		for(j = 0, k = width-1; j < width ; j++, k--){
				parca.boyut[i][j] = temp.boyut[k][i];
		}
	}
	parcaSil(temp);
}

void parcaDondur2( Parca parca )                //saat yonunun tersinde dondur.
{
	Parca temp = parcaKopya(parca);
	int i, j, k, width, height, tmp;
	width = parca.en;
    height = parca.boy;
	for(i = 0; i < width ; i++){
		for(j = 0, k = width-1; j < width ; j++, k--){
				parca.boyut[i][j] = temp.boyut[i][k];
		}
	}
    // parca.boy = parca.en;
    // parca.en = height;
	parcaSil(temp);
}

void parcaSabitle( int r, int c, char oyun[r][c] )      //parcayi oyun tahtasina sabitlemek icin.
{
    int i, j;
	for(i = 0; i < eldeParca.en ;i++){
		for(j = 0; j < eldeParca.en ; j++){
			if(eldeParca.boyut[i][j])
				oyun[eldeParca.x+i][eldeParca.y+j] = eldeParca.boyut[i][j];
		}
	}
}

void skorlama( int r, int c, char oyun[r][c])
{
	int i, j, sum, count=0;
	for(i=0 ; i<r ; i++){
		sum = 0;
		for(j=0 ; j<c ; j++) {
			sum+=oyun[i][j];
		}
		if(sum == c ){
			count++;
			int l, k;
			for(k = i;k >=1;k--)
				for(l=0 ; l<c ; l++)
					oyun[k][l]=oyun[k-1][l];
			for(l=0 ; l<c ; l++)
				oyun[k][l]=0;
		}
	}
	skor += 100*count*i;
}

void printleme( int r, int c, char oyun[r][c])
{
	char Buffer[r][c];
	int i, j;
    
    for ( i = 0; i < r; i++)
    {
        for ( j = 0; j < c; j++)
        {
            Buffer[i][j] = 0;
        }
    }
    
	for(i = 0; i < eldeParca.en ;i++){
		for(j = 0; j < eldeParca.en ; j++){
			if(eldeParca.boyut[i][j])
				Buffer[eldeParca.x+i][eldeParca.y+j] = eldeParca.boyut[i][j];
		}
	}

	system("cls");
	for(i=1; i<((c)/2) ; i++)
		printf("  ");

	printf("TETRIS\n");

    cursorSet(c+2-6,r+3+1);
    printf("△ Score: %d △\n", skor);

    cursorSet(c*2+4+1, 3-1);
    printf("▷ Controls:");
    cursorSet(c*2+4+1, 4-1);
    printf("  ▹ ↓ s  ▹ ← a  ▹ → d");
    cursorSet(c*2+4+1, 5-1);
    printf("  ▹ ROTATE e");
    cursorSet(c*2+4+1, 6-1);
    printf("  ▹ DROP x");
    cursorSet(c*2+4+1, 7-1);
    printf("  ▹ Coordinate drop 1-9");
    cursorSet(c*2+4+1, 8-1);
    printf("  ▹ Quit p");

    cursorSet(0,1);
    printf("╔══");
    for( i=1 ; i<c ; i++ ){
        printf("══");
    }
    printf("═╗\n");
	for(i = 0; i < r ;i++){
        printf("║ ");
		for(j = 0; j < c ; j++){
            if( Buffer[i][j])
                printf("□ ");
            else if( oyun[i][j] )
			    printf("%s ", "▣");
            else{
                printf(". ");
            }
		}
        printf("║");
		printf("\n");
	}
    printf("╚═");
    for( i=0 ; i<c ; i++ ){
        printf("══");
    }
    printf("╝");
}

void GameOver( int r, int c, char oyun[r][c])
{
	char Buffer[r][c];
	int i, j;
    
    for ( i = 0; i < r; i++)
    {
        for ( j = 0; j < c; j++)
        {
            Buffer[i][j] = 0;
        }
    }
    
	for(i = 0; i < eldeParca.en ;i++){
		for(j = 0; j < eldeParca.en ; j++){
			if(eldeParca.boyut[i][j])
				Buffer[eldeParca.x+i][eldeParca.y+j] = eldeParca.boyut[i][j];
		}
	}

	system("cls");
	for(i=1; i<((c)/2) ; i++)
		printf("  ");

	printf("TETRIS\n");


    cursorSet(c*2+4+1, 3-1);
    printf("△ Score: %d △\n", skor);    cursorSet(c*2+4+1, 4-1);
    // printf("  ▹ ↓ s  ▹ ← a  ▹ → d");
    // cursorSet(c*2+4+1, 5-1);
    // printf("  ▹ ROTATE e");
    // cursorSet(c*2+4+1, 6-1);
    // printf("  ▹ DROP x");
    // cursorSet(c*2+4+1, 7-1);
    // printf("  ▹ Coordinate drop 1-9");
    // cursorSet(c*2+4+1, 8-1);
    // printf("  ▹ Quit p");

    cursorSet(0,1);
    printf("╔══");
    for( i=1 ; i<c ; i++ ){
        printf("══");
    }
    printf("═╗\n");
	for(i = 0; i < r ;i++){
        printf("║ ");
		for(j = 0; j < c ; j++){
            if( Buffer[i][j])
                printf(". ");
            else if( oyun[i][j] )
			    printf("%s ", ".");
            else{
                printf("▣ ");
            }
		}
        printf("║");
		printf("\n");
	}
    printf("╚═");
    for( i=0 ; i<c ; i++ ){
        printf("══");
    }
    printf("╝");
}

void GameOver2( int r, int c, char oyun[r][c])
{
	char Buffer[r][c];
	int i, j;
    
    for ( i = 0; i < r; i++)
    {
        for ( j = 0; j < c; j++)
        {
            Buffer[i][j] = 0;
        }
    }
    
	for(i = 0; i < eldeParca.en ;i++){
		for(j = 0; j < eldeParca.en ; j++){
			if(eldeParca.boyut[i][j])
				Buffer[eldeParca.x+i][eldeParca.y+j] = eldeParca.boyut[i][j];
		}
	}

	system("cls");
	for(i=1; i<((c)/2) ; i++)
		printf("  ");

	printf("TETRIS\n");


    cursorSet(c*2+4+1, 3-1);
    printf("△ Score: %d △\n", skor);    cursorSet(c*2+4+1, 4-1);

    cursorSet(0,1);
    printf("╔══");
    for( i=1 ; i<c ; i++ ){
        printf("══");
    }
    printf("═╗\n");
	for(i = 0; i < r ;i++){
        printf("║ ");
		for(j = 0; j < c ; j++){
            if( Buffer[i][j])
                printf("▣ ");
            else if( oyun[i][j] )
			    printf("%s ", "▣");
            else{
                printf(". ");
            }
		}
        printf("║");
		printf("\n");
	}
    printf("╚═");
    for( i=0 ; i<c ; i++ ){
        printf("══");
    }
    printf("╝");
}

void temelFonk( char durum, int r, int c, char oyun[r][c])
{
    int i, j, a, en;
    en = 0;
    Parca tmp = parcaKopya( eldeParca );

    for( i=0 ; i<tmp.en ; i++ ){
        a=0;
        for( j=0 ; j<tmp.en ; j++){
            if( tmp.boyut[i][j] ){
                a++;
            }   
        }
        if( a > en){
            en = a;
        }
    }

    eldeParca.tmp_en = en;

    int x  = durum - '0';
    if( x > 0 && x < 10 )
    {
        x--;
        int max_i = c - tmp.tmp_en -1;
        i = x - tmp.y;
        if( i > max_i ) {
            tmp.y = max_i + 1;
            printf("1");
        } else {
            tmp.y = tmp.y + i;
            printf("2");
        }
        if(koordKontrol(tmp, r, c, oyun)){
            eldeParca.y = tmp.y;
        }
        printf("YES!");
        
        while( koordKontrol(eldeParca, r, c, oyun) )
        {
            eldeParca.x++;
        }
        eldeParca.x--;
        skorlama(r, c, oyun);
        parcaSabitle(r, c, oyun);
        parcaGetir(r, c, oyun);
    }

    switch( durum ) {
        case 'x':
            while( koordKontrol(eldeParca, r, c, oyun) )
            {
                eldeParca.x++;
            }
            eldeParca.x--;
            skorlama(r, c, oyun);
            parcaSabitle(r, c, oyun);
            parcaGetir(r, c, oyun);
            break;    
        case 'e':
            parcaDondur1(tmp);
            if( koordKontrol(tmp, r, c, oyun) )
                parcaDondur1(eldeParca);
                eldeParca.x = tmp.x;
                eldeParca.y = tmp.y;
            break;
        case 's':
            tmp.x++;
            if( koordKontrol(tmp, r, c, oyun) )
                eldeParca.x++;
            else
            {
                skorlama(r, c, oyun);
                parcaSabitle(r, c, oyun);
                parcaGetir(r, c, oyun);
            }       
            break;
        case 'd':
            tmp.y++;
            if(koordKontrol(tmp, r, c, oyun))
                eldeParca.y ++;
            break;
        case 'a':
            tmp.y--;
            if(koordKontrol(tmp, r, c, oyun))
                eldeParca.y--;
            break;
        
        case 'p':
            GameOn = FALSE;
            break;
        }

    parcaSil(tmp);
    skorlama(r, c, oyun);
    printleme(r, c, oyun);
}



void loadingScreen()
{
    HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO cursorInfo;
    SetConsoleOutputCP(CP_UTF8);

    // Get the current cursor info
    GetConsoleCursorInfo(consoleHandle, &cursorInfo);

    // Hide the cursor
    cursorInfo.bVisible = FALSE;
    SetConsoleCursorInfo(consoleHandle, &cursorInfo);

    system("cls");
    int i;
    int a = 0;

    int l = 5;
    int m = 5;
    int z = 2;
    
    for( i=0 ; i<10 ; i++)
    {
        cursorSet(l+20,m);
        printf("Loading...");
        usleep(200000);
        cursorSet(l+20,m);
        printf("           ");
        usleep(350000);

        cursorSet(a+20,m+1);
        printf("▄▄▄ ");
        a += 2;
        printf("%%%d", i*12);
        
        cursorSet(l+20,m);

    }

    system("cls");
}

void printGameOver(int c)
{
    int a = 5;
    cursorSet(c*2+4+1, a+1);
    printf("  _____                \n");
    cursorSet(c*2+4+1, a+2);
    printf(" / ___/__ ___ _  ___   \n");
    cursorSet(c*2+4+1, a+3);
    printf("/ (_ / _ `/  ' \\/ -_)  \n");
    cursorSet(c*2+4+1, a+4);
    printf("\\___/\\_,_/_/_/_/\\__/ __ \n");
    cursorSet(c*2+4+1, a+5);
    printf(" / __ \\_  _____ ____/ /\n");
    cursorSet(c*2+4+1, a+6);
    printf("/ /_/ / |/ / -_) __/_/ \n");
    cursorSet(c*2+4+1, a+7);
    printf("\\____/|___/\\__/_/ (_)   \n");
}

void tetrisText(int b, int a)
{
system("cls");
cursorSet(b, a+1);
printf(" _____ _____ _____ ____   ___ ____ \n");
cursorSet(b, a+2);
printf("|_   _| ____|_   _|  _ \\\\|_ _/ ___| \n");
cursorSet(b, a+3);
printf("  | | |  _|   | | | |_) || |\\___ \\  \n");
cursorSet(b, a+4);
printf("  | | | |___  | | |  _ < | | ___) | \n");
cursorSet(b, a+5);
printf("  |_| |_____| |_| |_| \\_\\___|____/   \n\n");
}


int main ()
{
    SetConsoleOutputCP(CP_UTF8);

    int r,c;
	int i, j;
    
    int a = 0;
    int b = 0;

    tetrisText(b, a);
    printf("Hosgeldiniz!\n");    
    printf("Sizden tahtanin boyutlari istenecektir.\n");    
    printf("Devam etmek icin herhangi bir tusa basiniz.\n");
    getChar();    
    printf("\n");
    printf("═════════════════════════════════════════════════════════════════════════════════════════════════════════════════════\n");
    printf("lutfen satir sayisini giriniz: ");
    scanf("%d", &r);
    printf("\n");
    printf("lutfen sutun sayisini giriniz: ");
    scanf("%d",&c);

    loadingScreen();

    char oyun[r][c];

    for ( i = 0; i < r ; i++)
    {
        for ( j = 0; j < c; j++)
        {
            oyun[i][j] = 0;
        }
        
    }

    srand(time(0));
    skor = 0;
    int durum;
	parcaGetir(r, c, oyun);
    printleme(r, c, oyun);
	while(GameOn){
		if ((durum = getChar()) != 0 ) {
			temelFonk(durum, r, c, oyun);
            skorlama(r, c, oyun);
		}
	}
	system("cls");
    for ( i=0; i < 7 ; i++)
    {
        GameOver(r, c, oyun);
        usleep(30000);
        printGameOver(c);
        usleep(50000);
        GameOver2(r, c, oyun);
        usleep(20000);
    }
    
    printGameOver(c);
    
    cursorSet(0,30);
    

    return 0;
}
