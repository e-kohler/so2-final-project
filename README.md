# P3

# User mode
Não conseguimos implementar a passagem para modo usuário na execução da aplicação. Após o salto para o modo usuário, a execução entrava em loop com uma interrupção de id=12 (Instruction page fault).
No código, está comentada a tentativa de configuração do registrador sstatus(bit 8 = 0) antes do sret executado no context->load().

Achamos que o erro possa ter relação com as page tables na construção dos Segments e Chunks, no entanto, não conseguimos chegar em uma solução em tempo. 
Deixamos também como comentário nossa tentativa de mitigar esse problema, para uma possível utilização futura.

# Syscalls
As sycalls estão acontecendo no modo supervisor (exceção 9).

# Handlers
Foram utilizados os handlers já implementados no Framework fornecido. 
Além desses, implementamos handlers para as classes Semaphore, Mutex e Condition.

# Test
O teste anexado (test.cc) contém exemplos de funcionalidades que geram syscalls.
Percebemos que o terminal integrado da nossa IDE não printou toda a execução em alguns casos, mesmo com o funcionamento correto do programa. Para isso recomendamos verificar o output da execução direto no arquivo test.out.