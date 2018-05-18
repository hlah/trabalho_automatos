#include "glc.h"
#include "options.h"

#include <iostream>


int main(int argc, char** argv) {


	// lê opções
	Options opts;
	opts.le_argumentos(argc, argv);

	if(argc <= 1) {
		opts.uso();
	}

	GLC glc;

	// carrega gramática
	if( opts.glc_entrada().size() != 0 ) {
		glc.carrega_arquivo(opts.glc_entrada());
	}
	// simplifica
	if( opts.simplificar() && !opts.normalizar() ) {
		glc.simplifica();
	}
	
	// normaliza
	if( opts.normalizar() ) {
		glc.normaliza();
	}

	// exibe gramática
	if( glc.aberto() && opts.exibir() && !opts.simplificar() && !opts.normalizar() ) {
		glc.exibe();
	}
	

	// verifica
	


	// salva gramática


	return 0;
}
