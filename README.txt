======== Autores =========
Guilherme Berlezi de Quadros 	00230326
Gustavo Oliva Barnasque			00263056
Mateus Davi Simon				00209411


======= Compilação =======
make 
executável salvo na pasta "bin/"


========== Uso ===========
glc arquivo_de_entrada [arquivo_de_saida] [opcoes]

opções:
 -p	exibe definição formal da gramática.
 -s	simplifica a gramática.
 -n	normaliza gramática para forma normal de Chomsky.
 -v [arquivo]	verifica se palavra no arquivo dado pertence a linguagem da gramática. 

obs: o arquivo a ser verificado deve estar num formato semelhante ao da
gramática, com cada simbolo entre colchetes, exemplo:

[ simbolo1 ][ simbolo2 ][ simbolo3 ]
