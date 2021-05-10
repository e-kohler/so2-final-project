# P4

# User mode
Conseguimos implementar a passagem para modo usuário na execução da aplicação. Para isso, setamos o registrador _st com SPP_U | SPIE | SUM no Context das threads de user, então, após o carregamento do contexto no Context::load(), a execução passará para user mode. Para as threads de kernel, o _st é setado com SPP_S | SPIE | SUM.

# Syscalls
Utilizamos como base o último commit disponibilizado pelo professor, mas adaptamos para uma versão mais parecida com o nosso envio do p3.
Para acessar funcionalidades do kernel, são geradas syscalls de mensagem pelo usuário, os quais são tratadas pelo handler de exceção.

# Mapeamento e proteção
Utilizamos um construtor de Chunk diferente para o remapeamento das áreas de memória mapeadas no setup. No thread_init(), os Segmentos criados são remapeads com flags de APP, o qual possui o bit 4 de usuário das Page-Flags, permitindo o acesso pelo modo usuário.

# Stack
No construtor da Thread, no método init_stack, um ponteiro para a stack de user é salvo em cima do objeto de Context da thread. Então, no Context::load() esse ponteiro é passado para o sp, de forma que a aplicação execute em modo usuário com acesso à stack de user.

# Dificuldades
Não conseguimos implementar uma solução para o switch_context a tempo. Realizamos testes principalmente com a aplicação hello, e no estado final, as syscalls de print funcionam, porém uma exceção 7 é gerada após a Thread::exit(), justamente pois essa gera um dispatch(), o qual invoca um switch_context().
Entendemos que deveríamos salvar o contexto das threads considerando a stack de usuário, para que a Thread que esteja entrando tenha um valor válido de sp antes do sret.
