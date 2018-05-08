#include "options.h"

#include <cstring>
#include <iostream>

// lê argumentos
bool Options::le_argumentos(int argc, char** argv) {
	bool success = true;
	// limpa
	_simplificar = false;
	_normalizar = false;
	_verificar = false;
	_glc_entrada = "";
	_glc_saida = "";
	_string_entrada = "";
	// lê cada argumento
	int i = 1;
	while(i < argc && success) {
		if( argv[i][0] == '-' ) {
			// verifica se opção é maior que 2 carácteres
			if(std::strlen(argv[i])>=2) {
				if(argv[i][1] == 's') {
					_simplificar = true;
				} else if(argv[i][1] == 'n') {
					_normalizar = true;
				} else if(argv[i][1] == 'v') {
					_verificar = true;
				} else if(argv[i][1] == 'i') {
					if( i+1 < argc ) {
						_string_entrada = argv[i+1];
					} else {
						std::cout << "Opcao -i exige parametro.\n";
						success = false;
					}
					i++;
				} else {
					std::cout << "Opcao '-" << argv[i][1] << "' invalida.\n";
					success = false;
				}
			} else {
				std::cout << "Opcao '-' invalida.\n";
				success = false;
			}
		} else {
			if( _glc_entrada.size() == 0 )
				_glc_entrada = argv[i];	
			else 
				_glc_saida = argv[i];	
		}
		i++;
	}
	return success;
}
