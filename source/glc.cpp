#include "glc.h"

#include <fstream>
#include <regex>

#include <iostream>

// carrega GLC de arquivo
bool GLC::carrega_arquivo(std::string arquivo_str) {
	bool success = false;
	// enumerador para o estado de leitura
	enum Estado {
		INICIO,
		TERMINAIS,
		VARIAVEIS,
		INICIAL,
		REGRAS,
		FINAL
	};
	// limpa variaveis
	_term_contador = 0;
	_vars_contador = 0;
	_int_para_term.clear();
	_term_para_int.clear();
	_int_para_var.clear();
	_var_para_int.clear();
	_regras.clear();
	// abre arquivo
	std::ifstream arquivo{arquivo_str};
	if( arquivo.is_open() ) {
		// compila expressões regulares
		std::regex re_estado_terminais{"\\s*#Terminais.*"};
		std::regex re_estado_variaveis{"\\s*#Variaveis.*"};
		std::regex re_estado_inicial{"\\s*#Inicial.*"};
		std::regex re_estado_regras{"\\s*#Regras.*"};
		std::regex re_simbolo{"\\[\\s*([\\w]+)\\s*\\]*"};
		std::regex re_regra{"\\s*\\[\\s*([\\w]+)\\s*\\]\\s*>\\s*([^#]+)"};

		std::smatch m;
		Estado estado = Estado::INICIO;
		std::string linha;
		bool erro = false;
		while(estado != Estado::FINAL && !erro) {
			// checa fim de arquivo
			if( arquivo.eof() ) {
				estado = Estado::FINAL;
			} else {
				// obtém linha
				std::getline(arquivo, linha);
			}
			// estado INICIO
			if(estado == Estado::INICIO) {
				if( std::regex_search(linha, re_estado_terminais) ) {
					estado = TERMINAIS;
				}
			}
			// estado TERMINAIS
			else if(estado == Estado::TERMINAIS) {
				// obtém terminais
				if( std::regex_search(linha, m, re_simbolo) ) {
					auto terminal = m[1].str();
					if( terminal == _vazio ) {
						std::cout << "ERRO: '" << _vazio << "' não pode ser terminal pois é símbolo de vazio.\n";
						erro = true;
					} else {
						// cria mapas
						_term_para_int[terminal] = -(++_term_contador);
						_int_para_term[-_term_contador] = terminal;
					}
				}
				// checa próxima seção
				if( std::regex_search(linha, re_estado_variaveis) ) {
					estado = Estado::VARIAVEIS;
				}
			}
			else if(estado == Estado::VARIAVEIS) {
				// obtém variaveis
				if( std::regex_search(linha, m, re_simbolo) ) {
					auto variavel = m[1].str();
					// checa se é simbolo de vazio
					if( variavel == _vazio ) {
						std::cout << "ERRO: '" << _vazio << "' não pode ser variavel pois é símbolo de vazio.\n";
						erro = true;
					} else {
						// cria mapas
						_var_para_int[variavel] = _vars_contador;
						_int_para_var[_vars_contador++] = variavel;
					}
				}
				// checa próxima seção
				if( std::regex_search(linha, re_estado_inicial) ) {
					estado = Estado::INICIAL;
				}
			}
			else if(estado == Estado::INICIAL) {
				// obtém variavel inicial
				if( std::regex_search(linha, m, re_simbolo) ) {
					auto inicial = m[1].str();
					// checa se inicialé variavel
					auto inicial_pos = _var_para_int.find(inicial);
					if( inicial_pos != _var_para_int.end() ) {
						// move para o inidice 0
						auto outro = _int_para_var[0];
						int pos = inicial_pos->second;
						_int_para_var[0] = inicial;
						_int_para_var[pos] = outro;
						_var_para_int[inicial] = 0;
						_var_para_int[outro] = pos;
					} else {
						std::cerr << "ERRO: simbolo inicial '" << inicial << "' não é variavel.";
						erro = true;
					}
				}
				// checa próxima seção
				if( std::regex_search(linha, re_estado_regras) ) {
					estado = Estado::REGRAS;
				}
			}
			else if(estado == Estado::REGRAS) {
				// checa regra
				if( std::regex_search(linha, m, re_regra) ) {
					auto cabeca = m[1].str();
					auto producao = m[2].str();
					//std::cout << cabeca << " --> " << producao << "\n";
					// checa se cabeca é variavel
					auto cabeca_it = _var_para_int.find(cabeca);
					if( cabeca_it == _var_para_int.end() ) {
						std::cout << "ERRO: Cabeça de produção não é variavel.\n";
						erro = true;
					} else {
						// obtém produção
						std::vector<int> simbolos;
						while( std::regex_search(producao, m, re_simbolo) ) {
							auto simbolo = m[1];
							// ignora símbolo vazio
							if( simbolo != _vazio ) {
								// checa se símbolo é variavel
								auto simbolo_it = _var_para_int.find(simbolo);
								if( simbolo_it != _var_para_int.end() ) {
									simbolos.push_back(simbolo_it->second);
								} else {
									// checa se é terminal
									simbolo_it = _term_para_int.find(simbolo);
									if( simbolo_it != _var_para_int.end() ) {
										simbolos.push_back(simbolo_it->second);
									} else {
										std::cout << "ERRO: símbolo de produção não é terminal, variavel ou símbolo de vazio.\n";
										erro = true;
									}
								}
							}
							// proximo simbolo
							producao = m.suffix().str();
						}
						// adiciona produção
						_regras[cabeca_it->second].push_back(simbolos);
					}
				}
			}
		}

		success = !erro;

		/*
		std::cout << "Mapa inteiros para terminais:\n";
		for( auto it = _int_para_term.begin(); it!=_int_para_term.end(); it++ )
			std::cout << it->first << ": " << it->second << "\n";
		std::cout << "Mapa terminais para inteiros:\n";
		for( auto it = _term_para_int.begin(); it!=_term_para_int.end(); it++ )
			std::cout << it->first << ": " << it->second << "\n";
		std::cout << "Mapa inteiros para variaveis:\n";
		for( auto it = _int_para_var.begin(); it!=_int_para_var.end(); it++ )
			std::cout << it->first << ": " << it->second << "\n";
		std::cout << "Mapa variaveis para inteiros:\n";
		for( auto it = _var_para_int.begin(); it!=_var_para_int.end(); it++ )
			std::cout << it->first << ": " << it->second << "\n";
		// */

		/*
		for( int i=0; i<_vars_contador; i++ ) {
			for( auto prod : _regras[i] ) {
				std::cout <<  "\t " << i << " -> ";
				for( int simbolo : prod ) {
					std::cout << simbolo << " ";
				}
				std::cout << "\n";
			}
		}
		*/
		

		// fecha arquivo
		arquivo.close();

	}

	// marca como aberto
	_aberto = success;

	return success;
}

// exibe GLC na tela
bool GLC::exibe() {
	std::cout << "GLC = (T, V, S, P)\nT = { ";
	// exibe terminais
	for(int i=1; i<_term_contador; i++) {
		std::cout << _int_para_term[-i] << ", ";
	}
	std::cout << _int_para_term[-_term_contador];
	std::cout << "}\nV = { ";
	// exibe variaveis
	for(int i=0; i<_vars_contador-1; i++) {
		std::cout << _int_para_var[i] << ", ";
	}
	std::cout << _int_para_var[_vars_contador-1];
	std::cout << "}\nS = ";
	// exibe inicial
	std::cout << _int_para_var[0] << "\n";
	// exibe programa
	std::cout << "P = { \n";
	for( int i=0; i<_vars_contador; i++ ) {
		for( auto prod : _regras[i] ) {
			std::cout <<  "\t " << _int_para_var[i] << " -> ";
			// checa se vazio 
			if( prod.size() == 0 ) {
				std::cout << _vazio;
			}
			// printa simbolos
			for( int simbolo : prod ) {
				if(simbolo < 0)
					std::cout << _int_para_term[simbolo]<< " ";
				else
					std::cout << _int_para_var[simbolo] << " ";
			}
			std::cout << "\n";
		}
	}
	std::cout << "}\n";

}
