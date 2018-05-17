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
		std::regex re_simbolo{"\\[\\s*([\\S]+)\\s*\\]*"};
		std::regex re_regra{"\\s*\\[\\s*([\\S]+)\\s*\\]\\s*>\\s*([^#]+)"};

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
						_regras[cabeca_it->second].insert(simbolos);
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
void GLC::exibe() {
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

    // etapa  1: variveis que constituem produções vazias
	std::set<int> variaveis_ve;
	// Inicializa Ve
	for( const auto& producoes : _regras ) {
        // Para toda produção
        for( const auto& prod : producoes.second ) {
            // Se tiver tamanho zero é produção vazia
            if( prod.size() == 0 ) {
                variaveis_ve.insert(producoes.first);
            }
        }
	}

	int ve_prev_tamanho;
	// Até Ve não aumentar de tamanho
	do {
        ve_prev_tamanho = variaveis_ve.size();
        for( const auto& producoes : _regras ) {
            // para todas produções
            for( const auto& prod : producoes.second ) {
                // procura as que geram simbolo vazio por tabela
				bool gera_vazio = true;
                for( auto simbolo : prod) {
                    if( variaveis_ve.count(simbolo) == 0) {
						// encontrou simbolo que não pertence a Ve
						gera_vazio = false;
						break;
                    }
                }
				if( gera_vazio ) {
					// insere cabeça em Ve
					variaveis_ve.insert(producoes.first);
					break;
				}
            }
        }
	} while( ve_prev_tamanho < variaveis_ve.size() );

	// etapa 2: exclusão de produções vazias
    std::map<int,std::set<std::vector<int>>> producoes_p1;

    // Inicializa P1
	// insere produções não vazias
	for( const auto& producoes : _regras ) {
		for( const auto& prod : producoes.second ) {
			if( prod.size() != 0 ) {
				producoes_p1[producoes.first].insert(prod);
			}
		}
	}

	// até P1 não aumentar de tamanho
	int p1_prev_tamanho;
	do {
		p1_prev_tamanho = producoes_p1.size();
		// para cada variavel com producoes
		for( auto& producoes : producoes_p1 ) {
			size_t producoes_size = producoes.second.size();
			// para cada producoes
			for( auto& prod : producoes.second ) {
				// para cada simbolo
				for( auto simbolo_it= prod.begin(); simbolo_it !=  prod.end(); simbolo_it++) {
					// se simbolo em Ve (produz palavra vazia)
					if( variaveis_ve.count(*simbolo_it) != 0  &&  prod.size() >= 2) {
						// cria nova producao
						std::vector<int> nova_prod(  prod.begin(), simbolo_it );
						nova_prod.insert(nova_prod.end(), simbolo_it+1,  prod.end());
						// adiciona as regras
						producoes.second.insert(nova_prod);
						producoes_size++;
					}
				}
			}
		}
	} while( p1_prev_tamanho < producoes_p1.size() );


	// etapa 3: adiciona produção vazia de necesário
	if( variaveis_ve.count(0) > 0 ) {
		producoes_p1[0].insert({});
	}

    _regras = producoes_p1;
}
// etapa 2: remoção de substituição de variaveis
void GLC::remove_subst_vars() {

	// fechos
	std::map<int,std::set<int>> fechos;

	// Etapa 1: fecho transitivo de cada variavel

	// para A pertencente a V
	for( auto A : _variaveis ) {
		// adiciona transicoes A -> B
		for( const auto& prod : _regras[A] ) {
			// se produção é da for A -> B
			if( prod.size() == 1 && prod[0] >= 0 && prod[0] != A) {
				// adicina ao fecho
				fechos[A].insert(prod[0]);
			}
		}
		// adiciona próximas transições até fecho não aumentar
		int fecho_A_tamanho;
		do {
			fecho_A_tamanho = fechos[A].size();
			// para todas variaveis já no fecho
			for( auto B : fechos[A] ) {
				// suas produções
				for( const auto& prod : _regras[A] ) {
					// no formato B -> C
					if( prod.size() == 1 && prod[0] >= 0 ) {
						// adicina ao fecho
						fechos[A].insert(prod[0]);
					}
				}
			}
		} while( fecho_A_tamanho < fechos[A].size() );
	}

	// Etapa 2: exclusão das produções que substituem variaveis

	// P1
	std::map<int,std::set<std::vector<int>>> producoes_p1;

	// inicializa P1
	for( const auto& producoes : _regras ) {
		for( const auto& prod : producoes.second ) {
			// se produção não for no formato A -> B
			if( prod.size() != 1 || prod[0] < 0 ) {
				// adiciona a P1
				producoes_p1[producoes.first].insert(prod);
			}
		}
	}

	// para todo A pertencente a V
	for( auto A : _variaveis ) {
		// para todo B pertencente ao fecho de A
		for( auto B : fechos[A]  ) {
			// para todas produções de B
			for( const auto& prod : _regras[B] ) {
				// que não estão no formato B -> C
				if( prod.size() != 1 || prod[0] < 0 ) {
					// adiciona a P1
					producoes_p1[A].insert(prod);
				}
			}
		}
	}

	// atualiza regras
	_regras = producoes_p1;
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

/////////////// NORMALIZAÇÃO /////////////////////

// converte para forma normal de Chomsky
bool GLC::normaliza() {
	// verifica arquivo aberto
	if( !_aberto ) {
		std::cout << "ERRO: nenhum GLC aberto para simplificação.\n";
		return false;
	}

	// etapa 1: normalizar
	simplifica();

	// etapa 2: substituir terminais em produçoes >= 2

	std::map<int,int> term_prod; // producoes de terminais (evita repetições)

	// P1
	std::map<int,std::set<std::vector<int>>> producoes_p1;

	// para cada variavel com produções
	for( const auto& producoes : _regras ) {
		// para cada produção
		for( const auto& prod : producoes.second ) {
			// se for maior ou igual a 2
			if( prod.size() >= 2 ) {
				// producao atualizada
				std::vector<int> prod_atualizada;
				// para cada simbolo
				for( auto simbolo : prod ) {
					// se for terminal
					if( simbolo < 0 ) {
						// checa se producao de terminal não existe
						if( term_prod.count(simbolo) == 0 ) {
							// cria variavel auxiliar
							term_prod[simbolo] = variavel_auxiliar("T_"+_int_para_term[simbolo]);
						}
						// adiciona variavel substituta
						prod_atualizada.push_back(term_prod[simbolo]);
					} else {
						// adiciona varivel
						prod_atualizada.push_back(simbolo);
					}
				}
				// adiciona producão modificada
				producoes_p1[producoes.first].insert(prod_atualizada);
			} else {
				// se não adiciona sem nehuma mudança
				producoes_p1[producoes.first].insert(prod);
			}
		}
	}
	// adiciona novas produções de terminais
	for( const auto& nova_prod : term_prod ) {
		producoes_p1[nova_prod.second].insert({nova_prod.first});
	}

	// Etapa 3: substituir pares de variaveis nas produções >= 3
	std::map<std::vector<int>,int> par_prod; // producoes de pares de variaveis (evita repetições)

	// P2
	std::map<int,std::set<std::vector<int>>> producoes_p2;

	// para cada variavel
	for( const auto& producoes : producoes_p1 ) {
		// para cada producão
		for( const auto& prod : producoes.second ) {
			// se for maior ou igual a 3
			if( prod.size() >= 3 ) {
				int cabeca_atual = producoes.first;
				for( int i=0; i<prod.size()-1; i++ ) {
					// pega sufixo
					std::vector<int> sufixo{prod.begin()+i+1, prod.end()};
					// se for par final apenas cria transição para ele e termina
					if( sufixo.size() == 1 ) {
						producoes_p2[cabeca_atual].insert({prod[i],prod[i+1]});
						break;
					}
					// se produção para sufixo já existe usa e termina
					if( par_prod.count(sufixo) != 0 ) {
						producoes_p2[cabeca_atual].insert({prod[i],par_prod[sufixo]});
						break;
					}
					// se não cria par e variavel auxiliar continua
					int nova_var = variavel_auxiliar("Aux");
					producoes_p2[cabeca_atual].insert({prod[i],nova_var});
					par_prod[sufixo] = nova_var;
					cabeca_atual = nova_var;
				}
			} else {
				// se for < 3 adiciona sem mudança
				producoes_p2[producoes.first].insert(prod);
			}
		}
	}


	// atualiza regras
	_regras = producoes_p2;


	return true;
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

// cria variavel auxiliar
int GLC::variavel_auxiliar( std::string nome_base ) {
	// procura novo nome
	int n = 0;
	if( _var_para_int.count(nome_base) != 0 ) {
		n = 1;
		while( _var_para_int.count(nome_base+std::to_string(n)) != 0 ) {
			n++;
		}
	}
	std::string nova_var_str;
	if( n==0 ) {
		nova_var_str = nome_base;
	} else {
		nova_var_str = nome_base+std::to_string(n);
	}
	// adicona
	int nova_var_int = _vars_contador++;
	_int_para_var[nova_var_int] = nova_var_str;
	_var_para_int[nova_var_str] = nova_var_int;
	_variaveis.insert(nova_var_int);

	return nova_var_int;
}
