# P4
# Switch Context
Implementamos os salvamentos e carregamentos de contextos de kernel no switch_context(). Antes do sret, atualizamos o sp com o valor salvo em cima do Context, que poder conter o stack pointer de kernel ou de user. No salvamento do contexto da thread, estamos setando o bit SPP para S, de forma que a thread execute a volta para o dispatch() e o IC::entry em modo supervisor. 
# User mode
Conseguimos implementar a passagem para modo usuário na execução da aplicação. Para isso, setamos o registrador _st com SPP_U | SPIE | SUM no Context das threads de user, então, após o carregamento do contexto no Context::load(), a execução passará para user mode. Para as threads de kernel, o _st é setado com SPP_S | SPIE | SUM.

# Syscalls
Utilizamos como base o último commit disponibilizado pelo professor, mas adaptamos para uma versão mais parecida com o nosso envio do p3.
Para acessar funcionalidades do kernel, são geradas syscalls de mensagem pelo usuário, os quais são tratadas pelo handler de exceção.

# Mapeamento e proteção
Utilizamos um construtor de Chunk diferente para o remapeamento das áreas de memória mapeadas no setup. No thread_init(), os Segmentos criados são remapeads com flags de APP, o qual possui o bit 4 de usuário das Page-Flags, permitindo o acesso pelo modo usuário.

# Stack
No construtor da Thread, no método init_stack, um ponteiro para a stack de user é salvo em cima do objeto de Context da thread. Então, no Context::load() esse ponteiro é passado para o sp, de forma que a aplicação execute em modo usuário com acesso à stack de user.

# Thread Exit
Para executar o Thread::exit(), adicionamos um tratador para a exceção 12. O valor de retorno está sendo salvo em uma variável estática no IC::entry() para evitar que esse valor esteja sujo na execução do Thread::exit().