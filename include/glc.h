#ifndef _GLC_H_
#define _GLC_H_

#include <string>
#include <map>
#include <vector>
#include <set>

class GLC {
	public:
		// carrega GLC de arquivo
		bool carrega_arquivo(std::string arquivo_str);
		// salva GLC em arquivo
		bool salva_arquivo(std::string arquivo_str);
		// exibe GLC na tela
		void exibe();
		// simplifica o GLC
		bool simplifica();
		// converte para forma normal de Chomsky
		bool normaliza();
		// checa se possou alguma gramática aberta
		inline bool aberto() { return _aberto; };
		// checa se gramática está simplificata
		bool simplificada();
		// checa se gramática está na forma normal de Chomsky
		bool normalizada();
		// verifica se palavra em arquivo pertence a grática
		bool verifica(std::string str_path);
	private:
		///// METODOS PRIVADOS /////
		//// SIMPLIFICAÇÃO
		// etapa 1: remoção de produções vazias
		void remove_prod_vazias();
		// etapa 2: remoção de substituição de variaveis
		void remove_subst_vars();
		// etapa 3: removção de símbolos inuteis
		void remove_simb_inuteis();
		//// VERIFICAÇÂO
		// função para obter indice de coordenadas de matriz triangular
		inline int get_indice(int row, int col) const { return col+((row+1)*row)/2; };
		// imprime arvores de derivação
		std::vector<std::string> imprime_arvores(const std::vector<std::set<int>>& tabela,
			   									 const std::vector<int>& palavra,
												 int depth=0, int var=0,
												 int row=0, int col=0) const;
		//// AUXLIARES
		// remove símbolo (e todas as produções que à contém)
		void remove_simbolo(int simbolo);
		// atualiza variaveis (remove variaveis que não estão no conjunto dado)
		void atualiza_variaveis( const std::set<int>& v1 );
		void atualiza_terminais( const std::set<int>& t1 ); 
		// cria variavel auxiliar
		int variavel_auxiliar( std::string nome_base );
		///// VARIAVEIS PRIVADAS /////
		// contador de terminais e variaveis
		int _term_contador = 0;
		int _vars_contador = 0;
		// conjunto de variaveis
		std::set<int> _variaveis;
		// conjunto de terminais
		std::set<int> _terminais;
		// mapas
		std::map<int,std::string> _int_para_term;
		std::map<std::string,int> _term_para_int;
		std::map<int,std::string> _int_para_var;
		std::map<std::string,int> _var_para_int;
		// regras
		std::map<int,std::set<std::vector<int>>> _regras;
		// símbolo vazio
		const std::string _vazio = "V";
		// booleanos
		bool _aberto = false;
};

#endif
