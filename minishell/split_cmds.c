#include "get_next_line.h"

int get_last_quote(int i, char c, char *str)
{
    while (str[i] != c && str[i] != '\0')
        i++;
    return i;
}

void    unset(t_list *shell)
{
    char **line;
    int i;

    i = 0;
    line = ft_split(shell->rest, ' ');
    if(line[0] != NULL)
    {
        int tab = 0;
        while(line[tab] != 0)
        {
            DeleteNode(line[tab], shell);
            tab++;
        }   
    }
}

int    found_n(int i, char *str)
{
    while (str[i] == 'n' && str[i])
        i++;
    while (str[i] == ' ' && str[i])
        i++;
    return i;
}

char    *check_n(t_list *shell)
{
    int     i;
    int     start;
    char    *ptr;

    start = 0;
    shell->g_n = 0;
    ptr = ft_strdup("");
    i = 0;
    while(shell->rest[i] != '\0')
    {
        while (shell->rest[i] == '-' && shell->rest[i + 1] == 'n')
        {
            ptr = ft_strjoin(ptr, ft_substr(shell->rest, start, i - start));
            i = found_n(i + 1, shell->rest);
            start = i;
            shell->g_n = 1;
        }
        if (shell->g_n != 0)
        {
            ptr = ft_strjoin(ptr, ft_substr(shell->rest, start, ft_strlen(shell->rest) - start));
            return ptr;
        }
        i++;
    }
    return shell->rest;
}

void    use_echo(t_list *shell)
{
    if (shell->g_n == 0)
    {
        ft_putstr(shell->rest);
        ft_putstr("\n");
    }
    else
        ft_putstr(shell->rest);
}

void    echo(t_list *shell)
{
    int pid = fork();
    if (pid == 0)
    {
        shell->rest = check_n(shell);
        use_echo(shell);
        close(shell->g_fd);
        //dup2(shell->stdio[1], 1);
        exit(0);
    }
    else if(pid > 0)
        wait(NULL);
}



void    conditions(t_list *shell, char **env)
{
    if (!(ft_strcmp(shell->f_cmd, "echo")))
        echo(shell);
    else if(!(ft_strcmp(shell->f_cmd, "env")))
        print_env(shell);
    else if(!(ft_strcmp(shell->f_cmd, "export")))
        export(shell);
    else if(!ft_strcmp(shell->f_cmd, "unset"))
        unset(shell);
    else
        exec_cmd(env, shell);
}

int     ft_check_pipe(t_list *shell)
{
    int i;
    int pipe;

    i = 0;
    pipe = 0;
    while (shell->buffer[i])
    {
        if (shell->buffer[i] == '"' || shell->buffer[i] == '\'')
            i = skip_quotes(i + 1, shell->buffer[i], shell->buffer);
        if (shell->buffer[i] == '|')
            pipe++;
        i++;
    }
    return (pipe);
}

t_list  *ft_split_pipe(t_list *shell,int p,int *reminder)
{
    static int i;
    while(shell->buffer[i])
    {
        if (shell->buffer[i] == '"' || shell->buffer[i] == '\'')
            i = skip_quotes(i + 1, shell->buffer[i], shell->buffer);
        if (shell->buffer[i] == '|')
        {
            shell->tab[p] = ft_substr(shell->buffer,*reminder,i - *reminder);
            i++;
            *reminder = i;
            break;
        }
        i++;
    }
    return shell;
}
void    ft_last_command(t_list *shell, int p, int *reminder)
{
    shell->tab[p] = ft_substr(shell->buffer,*reminder, ft_strlen(shell->buffer) - *reminder);
    printf("|%s|\n",shell->tab[p]);
    shell->tab[++p] = NULL;
}
int spawn_proc (int in, int out, t_list *shell)
{
  pid_t pid;

  if ((pid = fork ()) == 0)
    {
      if (in != 0)
        {
          dup2 (in, 0);
          close (in);
        }

      if (out != 1)
        {
          dup2 (out, 1);
          close (out);
        }

      return execvp (cmd->argv [0], (char * const *)cmd->argv);
    }

  return pid;
}

void    ft_create_childprocess(t_list *shell)
{

}
void    ft_initiate_pipe(t_list *shell, int pipes)
{
    int i;
    pid_t   pid;

    i = 0;
    shell->g_fd_pipe = malloc(sizeof(int) * (pipes + 1));
    while(pipes > i)
    {
        pipe(pipes * i + 2);
        i++;
    }
    i = 0;
    while(pipes + 1 > i)
    {
        // ft_create_childprocess();
        pid = fork();
        if (pid == 0)
        {
            ft_create_childprocess();
        }
    }
}

void    split_cmds(char **env, t_list *shell)
{
    int pipe;
    int reminder;
    int indice;

    indice = 0;
    reminder = 0;
    g_read = 0;
    pipe = ft_check_pipe(shell);
    shell->tab = malloc(sizeof(char *) * (pipe + 2));
    while(indice < pipe)
    {
        ft_split_pipe(shell,indice,&reminder);
        indice++;
    }
    ft_last_command(shell,indice,&reminder);
    ft_check_redr(shell);
    get_first_command(shell);
    get_rest_command(shell);
    conditions(shell, env);
}