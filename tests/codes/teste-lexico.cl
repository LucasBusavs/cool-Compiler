(* ==========================================
   Bloco de comentário de múltiplas linhas
   para testar se o lexer ignora isso corretamente.
   ========================================== *)

-- Comentário de linha única: Outro teste de descarte

class TestaLexer inherits IO {
    -- Atributos com tipos e inicializações distintas
    id_valido_123 : Int <- 42;
    texto_string  : String <- "Ola, mundo!\nTestando \"escape\" de aspas.";
    booleano_true : Bool <- true;
    booleano_false: Bool <- false;

    testar_operadores(a : Int, b : Int) : Object {
        let resultado : Int in {
            -- Teste de operadores aritméticos e de atribuição
            resultado <- (a + b) * 10 / 2 - 5;
            
            -- Teste de operadores lógicos e relacionais
            if a <= b then 
                out_string("Menor ou igual\n")
            else 
                if a < b then 
                    out_string("Menor\n")
                else 
                    if a = b then 
                        out_string("Igual\n")
                    else 
                        out_string("Maior\n")
                    fi
                fi
            fi;

            -- Teste do operador de negação complementar (tilde)
            resultado <- ~resultado;
        }
    };

    testar_palavras_chave() : Object {
        -- Teste de palavras-chave estruturais (case, loop, pool, new)
        case self of
            objeto : Main => out_string("Instancia de Main");
            objeto : Object => out_string("Objeto generico");
        esac
    };
};

(* 
   ======================================================
   CASOS DE ERRO LÉXICO (Opcional - Remova se quebrar o teste)
   ======================================================
   As linhas abaixo contêm caracteres inválidos no Cool. 
   Seu lexer deve reportar ERRO para cada um deles, em vez de crashar:
*)

#  -- Caractere inválido
$  -- Caractere inválido
_id_invalido -- No Cool, identificadores não podem começar com underline
"string sem fechar -- Deve gerar erro de EOF na string ou string mal formatada
