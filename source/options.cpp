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
	_exibir = false;
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
				} else if(argv[i][1] == 'p') {
					_exibir = true;
				} else if(argv[i][1] == 'v') {
					if( i+1 < argc ) {
						_string_entrada = argv[i+1];
						_verificar = true;
					} else {
						std::cout << "Opcao -v exige parametro.\n";
						uso();
						success = false;
					}
					i++;
				} else {
					std::cout << "Opcao '-" << argv[i][1] << "' invalida.\n";
					uso();
					success = false;
				}
			} else {
				std::cout << "Opcao '-' invalida.\n";
				uso();
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

// exibe uso 
void Options::uso() {
	std::cout << "Modo de uso:\n";
	std::cout << "\tglc arquivo_de_entrada [opcoes]\n";
	std::cout << "opcoes:\n";
	std::cout << " -p\texibe definição formal da gramática.\n";
	std::cout << " -s\tsimplifica a gramática. (nao implementado)\n";
	std::cout << " -n\tnormaliza gramática para forma normal de Chomsky. (nao implementado)\n\n";
	std::cout << " -v [arquivo]\tverifica se palavra no arquivo dado pertence a linguagem da gramática. (nao implementado)\n\n";
}
