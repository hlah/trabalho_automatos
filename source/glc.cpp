#include "glc.h"

#include <fstream>
#include <regex>
#include <algorithm>

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
	std::set<int> variaveis_v1;
	int prev_tamanho;
	// até V1 não aumentar de tamanho
	do {
		prev_tamanho = variaveis_v1.size();
		for( const auto& producoes : _regras ) {
			// não precisa checar variaveis já adicionadas
			if( variaveis_v1.count(producoes.first) == 0 ) {
				bool achou = false;
				// para cada produção
				for( const auto& prod : producoes.second ) {
					// verifica se possui terminal ou variavel em V1
					for( auto simbolo : prod ) {
						if( simbolo < 0 || variaveis_v1.count(simbolo) != 0 ) {
							achou = false;
							variaveis_v1.insert(producoes.first);
							break;
						}
					}
					if(achou)
						break;
				}
			}
		}
	} while ( prev_tamanho < variaveis_v1.size() );
	// atualiza variaveis
	atualiza_variaveis(variaveis_v1);

	// etapa 2: qualquer símbolo é atingível a partir do símbolo inicial
	std::set<int> variaveis_v2{0};
	std::set<int> terminais_t2;
	int v2_prev_tamanho;
	int t2_prev_tamanho;
	// até V2 e T2 não aumentarem de tamanho
	do {
		v2_prev_tamanho = variaveis_v2.size();
		t2_prev_tamanho = terminais_t2.size();
		// checa símbolos atingiveis a partir de variaveis em V2
		for( auto var : variaveis_v2 ) {
			// para cada produção 
			for( auto prod : _regras[var] ) {
				// adiciona os símbolos atingiveis
				for( auto simbolo : prod ) {
					if( simbolo < 0 ) {
						// terminal
						terminais_t2.insert(simbolo);
					} else {
						// variavel
						variaveis_v2.insert(simbolo);
					}
				}
			}
		}
	} while ( v2_prev_tamanho < variaveis_v2.size() || t2_prev_tamanho < terminais_t2.size() );
	// atualiz variaveis e terminais
	atualiza_variaveis(variaveis_v2);
	atualiza_terminais(terminais_t2);

}


//////////////// AUXILIARES /////////////////////

// remove símbolo (e todas as produções que à contém)
void GLC::remove_simbolo(int simbolo) {
	// remove do respectivo conjunto e mapas
	if( simbolo < 0 ) {
		_terminais.erase(simbolo);
		auto str = _int_para_term[simbolo];
		_term_para_int[str];
		_int_para_term[simbolo];
	} else {
		_variaveis.erase(simbolo);
		auto str = _int_para_var[simbolo];
		_var_para_int[str];
		_int_para_var[simbolo];
	}
	// remove produções
	_regras.erase(simbolo);
	for( auto& producoes : _regras ) {
		// checa se produção contém símbolo
		for( auto prod = producoes.second.begin(); prod != producoes.second.end(); ) {
			if( std::find( prod->begin(), prod->end(), simbolo ) != prod->end() ) {
				producoes.second.erase(prod);
			} else {
				prod++;
			}

		}
	}
}

// atualiza variaveis (remove variaveis que não estão no conjunto dado)
void GLC::atualiza_variaveis( const std::set<int>& v1 ) {
	for( auto var = _variaveis.cbegin(); var != _variaveis.cend(); ) {
		if( v1.count(*var) == 0 ) {
			auto remover = *var;
			var++;
			remove_simbolo(remover);
		} else {
			var++;
		}
	}
}

// atualiza variaveis (remove variaveis que não estão no conjunto dado)
void GLC::atualiza_terminais( const std::set<int>& t1 ) {
	for( auto term = _terminais.cbegin(); term != _terminais.cend(); ) {
		if( t1.count(*term) == 0 ) {
			auto remover = *term;
			term++;
			remove_simbolo(remover);
		} else {
			term++;
		}
	}
}
