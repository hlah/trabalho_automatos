#include "glc.h"

#include <fstream>
#include <regex>

#include <iostream>

///////////// CARREGAMENTO E EXIBIÇÃO //////////////

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
	_aberto = false;
	_term_contador = 0;
	_vars_contador = 0;
	_variaveis.clear();
	_terminais.clear();
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
						// adiciona terminal
						_terminais.insert(_term_para_int[terminal]);
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
						// adiciona variavel
						_variaveis.insert(_var_para_int[variavel]);
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

		// checa erro
		success = !erro;

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
	for( auto term : _terminais ) {
		std::cout << _int_para_term[term] << ", ";
	}
	std::cout << "}\nV = { ";
	// exibe variaveis
	for( auto var : _variaveis ) {
		std::cout << _int_para_var[var] << ", ";
	}
	std::cout << "}\nS = ";
	// exibe inicial
	std::cout << _int_para_var[0] << "\n";
	// exibe programa
	std::cout << "P = { \n";
	for( auto var : _variaveis ) {
		for( auto prod : _regras[var] ) {
			std::cout <<  "\t " << _int_para_var[var] << " -> ";
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

//////////////// SIMPLIFICAÇAO ///////////////////

// simplifica o GLC
bool GLC::simplifica() {
	// verifica arquivo aberto
	if( !_aberto ) {
		std::cout << "ERRO: nenhum GLC aberto para simplificação.\n";
		return false;
	}
	// etapa 1
	remove_prod_vazias();
	// etapa 2
	remove_subst_vars();
	// etapa 3
	remove_simb_inuteis();

	return true;
}

// etapa 1: remoção de produções vazias
void GLC::remove_prod_vazias() {
	std::cout << "Remoção de produções vazias não implementada.\n";
}
// etapa 2: remoção de substituição de variaveis
void GLC::remove_subst_vars() {
	std::cout << "Remoção de substituição de variaveis não implementada.\n";
}
// etapa 3: remoção de símbolos inuteis
void GLC::remove_simb_inuteis() {
	// etapa 1: qualquer variavel gera terminais
	
	// etapa 2: qualquer símbolo é atingível a partir do símbolo inicial
}
