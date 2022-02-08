#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include <time.h> //Para receber o horário local

typedef struct info_livro{
	char sigla; // [L]ivre, [E]mprestado, [R]eservado
	int reg; // registro do livro
}info_livro;

typedef struct aluno{
	char nome[80];
	char RA[7];
	int emprestado; // qtde de livros emprestados - Max=3
	int reservado; // qtde de livros reservados - Max =1
	info_livro tabela[4];
}aluno;

typedef struct info_aluno{
	char sigla;
	char RA[7];
	int dia_ret; // dia – máx=31
	int mes_ret; // mes: fev=28 dias
	int dia_dev; // mes: abril=jun=set=nov=30 dias
	int mes_dev; // os demais = 31 dias
}info_aluno;

typedef struct livro{
	int reg; // gerado automaticamente
	char titulo[80];
	char autor[80];
	info_aluno status[2];
}livro;

void aloca_aluno(aluno **p,int tam);
void aloca_livro(livro **p,int tam);
void cadastra_aluno(aluno *p);
void cadastra_livro(livro *p);
int verifica_RA(char *RA);
int verifica_aluno();
int verifica_livro();
void grava_aluno(aluno *p);
void grava_livro(livro *p);
void consultaaluno(aluno *p);
void consultalivro(livro *p);
void emprestimo_reserva(aluno *p_aluno,livro *p_livro);
void devolucao(aluno *p_aluno,livro *p_livro);
void atualiza(aluno *p_aluno,livro *p_livro,int pos_aluno,int pos_tabela);
void diaemes(livro *p,int ind,int op);
int calculamulta(livro *p_livro,livro *dataatual);
void mostraeatualiza(aluno *p_aluno,livro *p_livro,int pos_aluno);

int main(){
    int op;
    aluno *p_aluno=NULL;
    livro *p_livro=NULL;
    
    aloca_aluno(&p_aluno,1);
    aloca_livro(&p_livro,1);
    
    setlocale(LC_ALL, "Portuguese");
    do
    {
    	system("cls");
    	printf("\t\tBiblioteca Facens\n\n");
        printf("1-Cadastrar aluno\n2-Consultar situação do aluno\n3-Cadastrar Livro\n4-Consultar livro\n5-Emprestimo/reserva de livro\n6-Devolução de livro\n7-Sair\n\n");
        printf("Digite a opção desejada: ");
        scanf("%d",&op);
        fflush(stdin);
        switch (op)
        {
        case 1:
            cadastra_aluno(p_aluno);
			system("pause");
            break;

        case 2:
			consultaaluno(p_aluno);
			system("pause");
            break;

        case 3:
			cadastra_livro(p_livro);
			system("pause");
            break;

        case 4:
			consultalivro(p_livro);
			system("pause");
            break;
            
        case 5:
        	emprestimo_reserva(p_aluno,p_livro);
        	system("pause");
        	break;
        	
        case 6:
        	devolucao(p_aluno,p_livro);
        	system("pause");
        	break;
        
        default:
        	if(op!=7){
        		printf("\nOpcao não reconhecida!\nDigite novamente.\n");
            	system("pause");
			}
			break;
        }
    } while (op!=7);

    return 0;
}

void aloca_aluno(aluno **p,int tam)
{
    if((*p=(aluno *)realloc(*p,tam*sizeof(aluno)))==NULL)
    {
        printf("Erro na alocacao\nO programa será encerrado!\n");
        exit(1);
    }
}

void aloca_livro(livro **p,int tam){
	if((*p=(livro *)realloc(*p,tam*sizeof(livro)))==NULL)
    {
        printf("Erro na alocacao\nO programa será encerrado!\n");
        exit(1);
    }
}

void cadastra_aluno(aluno *p){
	int check,i;
	system("cls");
	printf("Cadastro de Aluno\n\n");
	printf("Nome do Aluno: ");
	gets(p->nome);
	fflush(stdin);
	printf("RA (7 dígitos): ");
	do{
		gets(p->RA);
		fflush(stdin);
		check=verifica_RA(p->RA);
		if(check==1){
			printf("\nRA já cadastrado, digite outro: ");
		}
	}while(check==1);
	p->emprestado=0;
	p->reservado=0;
	
	for(i=0;i<4;i++){
		(p->tabela+i)->sigla='L'; //Inicializa com sigla 'L'(Livre)
	}
	
	grava_aluno(p);
	
	printf("\nAluno cadastrado com sucesso\n\n");
}

void cadastra_livro(livro *p){
	int num_reg,i;
	p->reg=verifica_livro()+1;
	
	system("cls");
	printf("Cadastro de Livro\n\n");
	printf("Título do livro: ");
	gets(p->titulo);
	fflush(stdin);
	printf("Autor: ");
	gets(p->autor);
	fflush(stdin);
	
	for(i=0;i<2;i++){
		(p->status+i)->sigla='L';//Inicializa com sigla 'L'(Livre)
	}
	
	grava_livro(p);
	
	printf("\nLivro cadastrado com sucesso\n\n");
}

int verifica_RA(char *RA){
	FILE *fptr;
	aluno *p=NULL;
	aloca_aluno(&p,1);
	
	int i,quant=verifica_aluno();
	
	if((fptr=fopen("alunos.bin","rb"))==NULL){
		return 0;
	}
	else{
		for(i=0;i<quant;i++){
			fseek(fptr,i*sizeof(aluno),0);
			fread(p,sizeof(aluno),1,fptr);
			if(strcmp(RA,p->RA)==0){
				fclose(fptr);
				return 1;
			}
		}
		fclose(fptr);
		return 0;
	}
}

int verifica_aluno(){
	FILE *fptr;
	int cont=0;
	if((fptr=fopen("alunos.bin","rb"))==NULL){
		return cont;
	}
	else{
		fseek(fptr,0,2);
		cont=ftell(fptr)/sizeof(aluno);
		fclose(fptr);
		return cont;
	}
}

int verifica_livro(){
	FILE *fptr;
	int cont=0;
	if((fptr=fopen("livros.bin","rb"))==NULL){
		return cont;
	}
	else{
		fseek(fptr,0,2);
		cont=ftell(fptr)/sizeof(livro);
		fclose(fptr);
		return cont;
	}
}

void grava_aluno(aluno *p){
	FILE *fptr=NULL;
	if((fptr=fopen("alunos.bin","ab"))==NULL){
		printf("Erro ao abrir o arquivo bin\n");
	}
	else{
		fwrite(p,sizeof(aluno),1,fptr);
	}
	fclose(fptr);
}

void grava_livro(livro *p){
	FILE *fptr=NULL;
	if((fptr=fopen("livros.bin","ab"))==NULL){
		printf("Erro ao abrir o arquivo bin\n");
	}
	else{
		fwrite(p,sizeof(livro),1,fptr);
	}
	fclose(fptr);
}

void consultaaluno(aluno *p){
	FILE *fptr=NULL;
	int op,quant,i,j;
	char RA[7];
	quant=verifica_aluno();
	
	
	system("cls");
	
	if((fptr=fopen("alunos.bin","rb"))==NULL){
		printf("Erro ao abrir o arquivo bin\n");
	}
	
	else{
		printf("\nQue tipo de consulta deseja realizar?:");
		printf("\n\n1-Total (Todos os alunos)");
		printf("\n2-Parcial (Acesso através do RA)");
		printf("\n3-Voltar ao menu principal\n");
		scanf("%d",&op);
		fflush(stdin);
		
		if(op==1){
			for(i=0;i<quant;i++){
				fseek(fptr,i*sizeof(aluno),0);
				fread(p,sizeof(aluno),1,fptr);
				printf("\nNome: %s\nRA: %s\nLivros emprestados: %d\nLivros reservados: %d\n",p->nome,p->RA,p->emprestado,p->reservado);
				
				for(j=0;j<4;j++){
					if((p->tabela+j)->sigla=='L'){
						printf("Tabela %d: L - ----\n",j);
					}else{
						printf("Tabela %d: %c – %d\n",j,(p->tabela+j)->sigla,(p->tabela+j)->reg);
					}
				}
				
			}
			
		}else if(op==2){
			printf("Digite o RA desejado: ");
			gets(RA);
			fflush(stdin);
			for(i=0;i<quant;i++){
				fseek(fptr,i*sizeof(aluno),0);
				fread(p,sizeof(aluno),1,fptr);
				if(strcmp(RA,p->RA)==0){
					printf("\nNome: %s\nRA: %s\nLivros emprestados: %d\nLivros reservados: %d\n",p->nome,p->RA,p->emprestado,p->reservado);
					for(j=0;j<4;j++){
						if((p->tabela+j)->sigla=='L'){
							printf("Tabela %d: L - ----\n",j);
						}else{
							printf("Tabela %d: %c – %d\n",j,(p->tabela+j)->sigla,(p->tabela+j)->reg);
						}
					}
					break;
				}
			}
			if(i==quant)
			printf("\nNúmero de RA não encontrado");
			
		}
		printf("\n\n");
		fclose(fptr);
	}
}

void consultalivro(livro *p){
	FILE *fptr=NULL;
	int op,quant,i,check=0;
	char status,titulo[80];
	
	quant=verifica_livro();
	
	system("cls");
	
	if((fptr=fopen("livros.bin","rb"))==NULL){
		printf("Falha ao abrir o arquivo bin\n");
	}
	
	else{
		printf("\nQue tipo de consulta deseja realizar?:");
		printf("\n\n1-Total (Todos os livros)");
		printf("\n2-Por status (L, E ou R)");
		printf("\n3-Por título");
		printf("\n4-Voltar ao menu principal\n");
		scanf("%d",&op);
		fflush(stdin);
		
		if(op==1){
			for(i=0;i<quant;i++){
				fseek(fptr,i*sizeof(livro),0);
				fread(p,sizeof(livro),1,fptr);
				printf("\nRegistro: %.3d\nTítulo: %s\nAutor: %s\n",p->reg,p->titulo,p->autor);
				printf("Status 0 - %c",(p->status+0)->sigla);
				if((p->status+0)->sigla!='L'){
					printf("\tRA: %s\tRet: %d/%d\tDev: %d/%d\n",(p->status+0)->RA,(p->status+0)->dia_ret,(p->status+0)->mes_ret,(p->status+0)->dia_dev,(p->status+0)->mes_dev);
				}else
				printf("\n");
				printf("Status 1 - %c",(p->status+1)->sigla);
				if((p->status+1)->sigla!='L'){
					printf("\tRA: %s\tRet: %d/%d\tDev: %d/%d\n",(p->status+1)->RA,(p->status+1)->dia_ret,(p->status+1)->mes_ret,(p->status+1)->dia_dev,(p->status+1)->mes_dev);
				}else
				printf("\n");
			}
		}
		else if(op==2){
			printf("Qual status deseja? (L, E ou R): ");
			scanf("%c",&status);
			fflush(stdin);
			for(i=0;i<quant;i++){
				fseek(fptr,i*sizeof(livro),0);
				fread(p,sizeof(livro),1,fptr);
				if(toupper(status)==(p->status+0)->sigla||toupper(status)==(p->status+1)->sigla){
					printf("\nRegistro: %.3d\nTítulo: %s\nAutor: %s\n",p->reg,p->titulo,p->autor);
					printf("Status 0 - %c",(p->status+0)->sigla);
					if((p->status+0)->sigla!='L'){
						printf("\tRA: %s\tRet: %d/%d\tDev: %d/%d\n",(p->status+0)->RA,(p->status+0)->dia_ret,(p->status+0)->mes_ret,(p->status+0)->dia_dev,(p->status+0)->mes_dev);
					}else
					printf("\n");
					printf("Status 1 - %c",p->status[0].sigla);
					if((p->status+1)->sigla!='L'){
						printf("\tRA: %s\tRet: %d/%d\tDev: %d/%d\n",(p->status+1)->RA,(p->status+1)->dia_ret,(p->status+1)->mes_ret,(p->status+1)->dia_dev,(p->status+1)->mes_dev);
					}else
					printf("\n");
					check=1;
				}
			}
			if(check==0)
			printf("\nNenhum livro com o status '%c' encontrado",status);
		}
		else if(op==3){
			printf("Digite o título do livro: ");
			gets(titulo);
			fflush(stdin);
			for(i=0;i<quant;i++){
				fseek(fptr,i*sizeof(livro),0);
				fread(p,sizeof(livro),1,fptr);
				if(strcmp(titulo,p->titulo)==0){
					printf("\nRegistro: %.3d\nTítulo: %s\nAutor: %s\n",p->reg,p->titulo,p->autor);
					printf("Status 0 - %c",p->status[0].sigla);
					if((p->status+0)->sigla!='L'){
						printf("\tRA: %s\tRet: %d/%d\tDev: %d/%d\n",(p->status+0)->RA,(p->status+0)->dia_ret,(p->status+0)->mes_ret,(p->status+0)->dia_dev,(p->status+0)->mes_dev);
					}else
					printf("\n");
					printf("Status 1 - %c",p->status[0].sigla);
					if((p->status+1)->sigla!='L'){
						printf("\tRA: %s\tRet: %d/%d\tDev: %d/%d\n",(p->status+1)->RA,(p->status+1)->dia_ret,(p->status+1)->mes_ret,(p->status+1)->dia_dev,(p->status+1)->mes_dev);
					}else
					printf("\n");
				}	
			}
			if(i==quant)
			printf("\nLivro não encontrado");
		}
		printf("\n\n");
		fclose(fptr);
	}
}

void emprestimo_reserva(aluno *p_aluno,livro *p_livro){
	FILE *fptr;
	char RA[7],titulo[80];
	int check,i,contaluno,contlivro,pos;
	contaluno=verifica_aluno();
	contlivro=verifica_livro();
	system("cls");
	printf("Insira o RA: ");
	do{
		gets(RA);
		fflush(stdin);
		check=verifica_RA(RA);
		if(check==0){
			printf("\nNúmero de RA não registrado. Insira um RA válido: ");
		}
	}while(check==0);
	
	if((fptr=fopen("alunos.bin","rb"))==NULL){
		printf("Falha ao abrir arquivo alunos.bin\n");
	}
	else{
		for(i=0;i<contaluno;i++){
			fseek(fptr,i*sizeof(aluno),0);
			fread(p_aluno,sizeof(aluno),1,fptr);
			if(strcmp(RA,p_aluno->RA)==0){
				pos=i;
				break;
			}
		}
		fclose(fptr);
	}
	
	if(p_aluno->emprestado<3||p_aluno->reservado<1){
		printf("Título do livro a ser emprestado/reservado: ");
		gets(titulo);
		fflush(stdin);
		
		if((fptr=fopen("livros.bin","rb"))==NULL){
			printf("Falha ao abrir arquivo livros.bin\n");
		}
		else{
			for(i=0;i<contlivro;i++){
				fseek(fptr,i*sizeof(livro),0);
				fread(p_livro,sizeof(livro),1,fptr);
				if(strcmp(titulo,p_livro->titulo)==0){
					break;
				}
			}
			fclose(fptr);
			if(i==contlivro){
				printf("Livro não encontrado\n");
				system("pause");
				main();
			}
			if((p_livro->status+0)->sigla=='L'){
				if(p_aluno->emprestado<3){
					(p_livro->status+0)->sigla='E';
					strcpy((p_livro->status+0)->RA,RA);
					diaemes(p_livro,0,0);
					for(i=0;i<3;i++){
						if((p_aluno->tabela+i)->sigla=='L'){
							p_aluno->emprestado++;
							(p_aluno->tabela+i)->sigla = 'E';
							(p_aluno->tabela+i)->reg = p_livro->reg;
							break;
						}
					}
					system("cls");
					printf("Livro emprestado com sucesso!\n\n");
					mostraeatualiza(p_aluno,p_livro,pos);
				}
				else if(p_aluno->reservado<1){
					(p_livro->status+0)->sigla='R';
					strcpy((p_livro->status+0)->RA,RA);
					diaemes(p_livro,1,1);
					p_aluno->reservado++;
					(p_aluno->tabela+3)->sigla = 'R';
					(p_aluno->tabela+3)->reg = p_livro->reg;
					system("cls");
					printf("Livro reservado com sucesso!\n\n");
					mostraeatualiza(p_aluno,p_livro,pos);
				}	
			}
			else if((p_livro->status+0)->sigla=='E'&&p_aluno->reservado<1){
				if((p_livro->status+1)->sigla=='R'){
					printf("Livro não disponível para reserva\n");
					printf("Emprestado para o RA: %s\n",(p_livro->status+0)->RA);
					printf("Data de devolução: %d/%d\n",(p_livro->status+1)->dia_dev,(p_livro->status+1)->mes_dev);
				}
				else if((p_livro->status+1)->sigla=='L'){
						(p_livro->status+1)->sigla='R';
						strcpy((p_livro->status+1)->RA,RA);
						diaemes(p_livro,1,1);
						p_aluno->reservado++;
						(p_aluno->tabela+3)->sigla = 'R';
						(p_aluno->tabela+3)->reg = p_livro->reg;
						system("cls");
						printf("Livro reservado com sucesso!\n\n");
						mostraeatualiza(p_aluno,p_livro,pos);
					}
				}
				else{
				printf("Quantidade máxima de livros reservados\n");
				}
			}		
		}
		else{
			printf("Quantidade máxima de livros emprestados e reservados\n");
		}
}

void devolucao(aluno *p_aluno,livro *p_livro){
	FILE *fptr;
	livro *dataatual=malloc(sizeof(livro));
	char RA[7],titulo[80];
	int check,i,j,contaluno,contlivro,pos;
	float multa;
	contaluno=verifica_aluno();
	contlivro=verifica_livro();
	
	system("cls");
	printf("Insira o RA: ");
	do{
		gets(RA);
		fflush(stdin);
		check=verifica_RA(RA);
		if(check==0){
			printf("\nNúmero de RA não registrado. Insira um RA válido: ");
		}
	}while(check==0);
	
	if((fptr=fopen("alunos.bin","rb"))==NULL){
		printf("Falha ao abrir arquivo alunos.bin\n");
	}
	else{
		for(i=0;i<contaluno;i++){
			fseek(fptr,i*sizeof(aluno),0);
			fread(p_aluno,sizeof(aluno),1,fptr);
			if(strcmp(RA,p_aluno->RA)==0){
				pos=i;
				break;
			}
		}
		fclose(fptr);
	}

	if(p_aluno->emprestado==0){
		printf("Aluno com nenhum livro emprestado\n");
		system("pause");
		main();
	}
	
	printf("\nDigite o título do livro a ser devolvido: ");
	gets(titulo);
	fflush(stdin);
	if((fptr=fopen("livros.bin","rb"))==NULL){
		printf("Falha ao abrir arquivo livros.bin\n");
	}
	else{
		for(i=0;i<contlivro;i++){
			fseek(fptr,i*sizeof(livro),0);
			fread(p_livro,sizeof(livro),1,fptr);
			if(strcmp(titulo,p_livro->titulo)==0){
				break;
			}
		}
		fclose(fptr);

		for(j=0;j<4;j++){
			if((p_aluno->tabela+j)->reg==p_livro->reg){
				break;
			}
		}

		if(j==4){
			printf("Livro não emprestado pelo aluno\n");
			system("pause");
			main();
		}

	}

	diaemes(dataatual,0,0);

	printf("\nData para a devolução: %d/%d\tData atual: %d/%d\n",(p_livro->status+0)->dia_dev,(p_livro->status+0)->mes_dev,(dataatual->status+0)->dia_ret,(dataatual->status+0)->mes_ret);
	multa=calculamulta(p_livro,dataatual);
	if(multa>0){
		printf("Livro devolvido %.0f dias depois do prazo. Multa = R$ %.2f\n",multa,multa*3.00);
	}
	else{
		atualiza(p_aluno,p_livro,pos,j);
		printf("Livro devolvido!\n");
	}

}

void atualiza(aluno *p_aluno,livro *p_livro,int pos_aluno,int pos_tabela){
	FILE *fptr;
	aluno *p_alunoR = malloc(sizeof(aluno));
	int i, contaluno=verifica_aluno(),pos,j;
	
	p_aluno->emprestado-=1;
	
	if((p_livro->status+1)->sigla == 'R'){
		if((fptr=fopen("alunos.bin","rb"))==NULL){
			printf("Erro ao abrir arquivo alunos.bin\n");
			exit(1);
		}
		
		for(j=0;j<contaluno;j++){
			fseek(fptr,j*sizeof(aluno),0);
			fread(p_alunoR,sizeof(aluno),1,fptr);
			if(strcmp((p_livro->status+1)->RA,p_alunoR->RA)==0){
				for(i=0;i<3;i++){
					if((p_alunoR->tabela+i)->sigla=='L'){
						(p_livro->status+1)->sigla = 'L';
						(p_livro->status+0)->sigla = 'E';
						strcpy((p_livro->status+0)->RA,p_alunoR->RA);
						p_alunoR->reservado-=1;
						p_alunoR->emprestado+=1;
						(p_alunoR->tabela+i)->sigla='E';
						(p_alunoR->tabela+i)->reg=p_livro->reg;
						(p_alunoR->tabela+3)->sigla='L';
						(p_alunoR->tabela+3)->reg=-1;
						i=3;
					}
				}
				pos=j;
				j=contaluno;
			}
		}
		fclose(fptr);
		fptr=fopen("alunos.bin","rb+");
		fseek(fptr,pos*sizeof(aluno),0);
		fwrite(p_alunoR,sizeof(aluno),1,fptr);
		fclose(fptr);
	}
	else{
		(p_livro->status+0)->sigla = 'L';
	}
	(p_aluno->tabela+pos_tabela)->sigla = 'L';
	(p_aluno->tabela+pos_tabela)->reg = -1;
	fptr=fopen("alunos.bin","rb+");
	fseek(fptr,pos_aluno*sizeof(aluno),0);
	fwrite(p_aluno,sizeof(aluno),1,fptr);
	fclose(fptr);
	
	if((fptr=fopen("livros.bin","rb+"))==NULL){
		printf("Erro ao abrir arquivo livros.bin\n");
		exit(1);
	}
	fseek(fptr,p_livro->reg-1*sizeof(livro),0);
	fwrite(p_livro,sizeof(livro),1,fptr);
	
	fclose(fptr);
	
	printf("\nRegistro: %.3d\nTítulo: %s\nAutor: %s\n",p_livro->reg,p_livro->titulo,p_livro->autor);
	printf("Status 0 - %c",(p_livro->status+0)->sigla);
	if((p_livro->status+0)->sigla!='L'){
		printf("\tRA: %s\tRet: %d/%d\tDev: %d/%d\n",(p_livro->status+0)->RA,(p_livro->status+0)->dia_ret,(p_livro->status+0)->mes_ret,(p_livro->status+0)->dia_dev,(p_livro->status+0)->mes_dev);
	}else
		printf("\n");
		printf("Status 1 - %c",(p_livro->status+1)->sigla);
		if((p_livro->status+1)->sigla!='L'){
			printf("\tRA: %s\tRet: %d/%d\tDev: %d/%d\n",(p_livro->status+1)->RA,(p_livro->status+1)->dia_ret,(p_livro->status+1)->mes_ret,(p_livro->status+1)->dia_dev,(p_livro->status+1)->mes_dev);
		}else
		printf("\n");
		
	printf("\nNome: %s\nRA: %s\nLivros emprestados: %d\nLivros reservados: %d\n",p_aluno->nome,p_aluno->RA,p_aluno->emprestado,p_aluno->reservado);
	for(j=0;j<4;j++){
		if((p_aluno->tabela+j)->sigla=='L'){
			printf("Tabela %d: L - ----\n",j);
		}else{
			printf("Tabela %d: %c – %d\n",j,(p_aluno->tabela+j)->sigla,(p_aluno->tabela+j)->reg);
		}
	}
}

int calculamulta(livro *p_livro,livro *dataatual){
	int quantmes[12] = {31,28,31,30,31,30,31,31,30,31,30,31};
	int i;
	float dias=0;
	if((p_livro->status+0)->mes_dev==(dataatual->status+0)->mes_ret){
		dias = (dataatual->status+0)->dia_ret - (p_livro->status+0)->dia_dev;
	}
	else{
		dias+= quantmes[(p_livro->status+0)->mes_dev-1] - (p_livro->status+0)->dia_dev;
		for(i=(dataatual->status+0)->mes_ret;i<(p_livro->status+0)->mes_dev-1;i++){
		dias+=quantmes[i];
		}
		dias+=(dataatual->status+0)->dia_ret;
	}
	
	return dias;
}

void diaemes(livro *p,int ind,int op){
	int mesret,mesdev;
	struct tm *data_hora_atual;  
	
	if(op==0){
		time_t segundos;
	    time(&segundos);   
	    data_hora_atual = localtime(&segundos);   
	    (p->status+ind)->dia_ret = data_hora_atual->tm_mday;  
	    (p->status+ind)->mes_ret = data_hora_atual->tm_mon+1;
	}   
	else if(op==1){
		(p->status+ind)->dia_ret = (p->status+ind-1)->dia_dev;
		(p->status+ind)->mes_ret = (p->status+ind-1)->mes_dev;
	}
	
	if((p->status+ind)->mes_ret == 2 && (p->status+ind)->dia_ret > 21){
	    	(p->status+ind)->mes_dev = 3;
	    	(p->status+ind)->dia_dev= (p->status+ind)->dia_ret + 7 - 28;
		}
	else if (((p->status+ind)->mes_ret == 2 || (p->status+ind)->mes_ret == 6 || (p->status+ind)->mes_ret == 9 || (p->status+ind)->mes_ret == 11) && (p->status+ind)->dia_ret >23){
		(p->status+ind)->mes_dev = (p->status+ind)->mes_ret + 1;
    	(p->status+ind)->dia_dev= (p->status+ind)->dia_ret + 7 - 30;
	}
	else if((p->status+ind)->mes_ret == 12 && (p->status+ind)->dia_ret >24){
		(p->status+ind)->mes_dev = 1;
		(p->status+ind)->dia_dev= (p->status+ind)->dia_ret + 7 - 31;
	}
    else if((p->status+ind)->dia_ret >24){
    	(p->status+ind)->mes_dev = (p->status+ind)->mes_ret + 1;
    	(p->status+ind)->dia_dev= (p->status+ind)->dia_ret + 7 - 31;
	}
	else{
		(p->status+ind)->mes_dev = (p->status+ind)->mes_ret;
    	(p->status+ind)->dia_dev = (p->status+ind)->dia_ret + 7;
	}	
}

void mostraeatualiza(aluno *p_aluno,livro *p_livro,int pos_aluno){
	FILE *fptr;
	aluno *alu;
	int contaluno,j;
	contaluno=verifica_aluno();

	
	printf("\nRegistro: %.3d\nTítulo: %s\nAutor: %s\n",p_livro->reg,p_livro->titulo,p_livro->autor);
	printf("Status 0 - %c",(p_livro->status+0)->sigla);
	if((p_livro->status+0)->sigla!='L'){
		printf("\tRA: %s\tRet: %d/%d\tDev: %d/%d\n",(p_livro->status+0)->RA,(p_livro->status+0)->dia_ret,(p_livro->status+0)->mes_ret,(p_livro->status+0)->dia_dev,(p_livro->status+0)->mes_dev);
	}else
	printf("\n");
	printf("Status 1 - %c",(p_livro->status+1)->sigla);
	if((p_livro->status+1)->sigla!='L'){
		printf("\tRA: %s\tRet: %d/%d\tDev: %d/%d\n",(p_livro->status+1)->RA,(p_livro->status+1)->dia_ret,(p_livro->status+1)->mes_ret,(p_livro->status+1)->dia_dev,(p_livro->status+1)->mes_dev);
	}else
	printf("\n");
	
	printf("\nNome: %s\nRA: %s\nLivros emprestados: %d\nLivros reservados: %d\n",p_aluno->nome,p_aluno->RA,p_aluno->emprestado,p_aluno->reservado);
				
	for(j=0;j<4;j++){
		if((p_aluno->tabela+j)->sigla=='L'){
			printf("Tabela %d: L - ----\n",j);
		}else{
			printf("Tabela %d: %c – %d\n",j,(p_aluno->tabela+j)->sigla,(p_aluno->tabela+j)->reg);
		}
	}
	
	if((fptr=fopen("livros.bin","rb+"))==NULL){
		printf("Falha ao abrir o arquivo livros.bin\n");
	}
	else{
		fseek(fptr,(p_livro->reg-1)*sizeof(livro),0);
		fwrite(p_livro,sizeof(livro),1,fptr);
		fclose(fptr);
	}
	
    if((fptr=fopen("alunos.bin","rb+"))==NULL){
		printf("Falha ao abrir o arquivo alunos.bin\n");
	}
	else{
		fseek(fptr,pos_aluno*sizeof(aluno),0);
		fwrite(p_aluno,sizeof(aluno),1,fptr);
		fclose(fptr);
	}
}
