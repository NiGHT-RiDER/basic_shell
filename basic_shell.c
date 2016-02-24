#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/wait.h>

#define TOK_TAILLE 64
#define TOK_SEPARATEUR " \n\t"

/*
  fonction intergres au pseudo shell
 */
int shell_cd(char **args);
int shell_help(char **args);
int shell_exit(char **args);

/*
   liste des fonctions qui est donne a un tableau de fonction 
   plus facile pour la maintenance :3
 */
char *fonctions_str[] = {
	"cd",
	"help",
	"exit"
};

int (*fonctions_integres[]) (char **)={
	&shell_cd,
	&shell_help,
	&shell_exit
};

int shell_nb_fct_integres(){
	return sizeof(fonctions_str) / sizeof(char *);
}

/*
 * implementation des fonction_integres
 */
int shell_cd(char **args){
	if(args[1] == NULL){
		fprintf(stderr , "shell attendait un argument a \"cd\"\n");
	} else {
		if(chdir(args[1]) != 0){
			perror("muh shell");
		}
	}
	return 1;
}

int shell_help(char**args){
	int i;
	printf("Le glorieux shell de Stefan Bogdanovic :3\n");
	printf("Tapez le nom du programme et appuyer sur enter \n");
	printf("Les commandes suivantes sont integres au shell ");

	for(i = 0 ; i<  shell_nb_fct_integres() ; i++){
		printf(" %s\n" , fonctions_str[i]);
	}
	printf("pour les autres commandes veuillez vous referrer au man :) \n");
	return 1;
}


int shell_exit(char **args){
	return 0;
}

int shell_execute(char **args){
	int i ;

	if(args[0] == NULL){
		// commande vide inserer
		return 1;
	}

	for(i = 0 ; i < shell_nb_fct_integres() ; i++){
		if(strcmp(args[0] , fonctions_str[i]) == 0){
			return(*fonctions_integres[i])(args);
		}
	}
	return shell_launcher(args);
}

int shell_launcher(char **args){
	int status;
	pid_t pid , wpid;

	// j'ai fork mon processus et je recupere le pid de ce dernier
	pid = fork();
	
	//process enfant
	if(pid == 0){
		// remplace le process courant par celui en parametre
		if(execvp(*args , args) == -1){
			perror("muh shell launcher");
		}
		exit(EXIT_FAILURE);
	}
	// erreur de fork
	else if (pid < 0){
		perror("muh fork");	
	} 
	// process parent
	else {
		do{
			wpid = waitpid(pid , &status , WUNTRACED);
		}while(!WIFEXITED(status) && !WIFSIGNALED(status));
	}
	return 1;
}

//methode qui decoupe notre line et la renvoie dans un tableau fini par un null
char **shell_split_line(char *line){
	char **tokens , *token;
	int buffer_size = TOK_TAILLE , position = 0;

	if((tokens= (char **)malloc(buffer_size * sizeof(char *))) == NULL){
		perror("Malloc failed");
		exit(EXIT_FAILURE);
	}

	for(token = strtok(line, TOK_SEPARATEUR) ; token != NULL ; token = strtok(NULL , TOK_SEPARATEUR) ){
		tokens[position++] = token;	
		if(position > buffer_size){
			buffer_size += TOK_TAILLE;
			if((tokens = realloc(tokens , buffer_size * sizeof(char*)))== NULL){
				perror("Realloc failure !");
				exit(EXIT_FAILURE);
			}	
		}
	} 
	tokens[position] = NULL;
	return tokens;
}

//methode qui lis une line sur stdin et la retourne
char *shell_read_line(){
	char *line = NULL;
	size_t size = 0;
	getline(&line , &size , stdin);
	return line;	
}

//methode qui fera tourner notre interpreteur 
void shell_loop(){
	char *line , **args;
	int status;

	do{
		printf("> ");
		line = shell_read_line();
		args = shell_split_line(line);
		status = shell_execute(args);

		//liberer la memoire de la line et des args
		free(line);
		free(args);
	} while(status);

}

int main(int argc , char **argv){
	/*charger les fichiers de config si il y en a */

	/*faire tourner la repetitive des commandes */
	shell_loop();

	/*nettoyer derriere sois si besoin est*/
	return EXIT_SUCCESS; // provient de stdlib.h
}
