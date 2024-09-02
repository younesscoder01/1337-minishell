/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rbenmakh <rbenmakh@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/03 14:43:12 by rbenmakh          #+#    #+#             */
/*   Updated: 2024/09/02 14:43:22 by rbenmakh         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

int check_nl(char *s)
{
    while(*s)
    {
        if(*s != 'n')
            return(1);
        s++;
    }
    return(0);
}
//fix the new line opriotn
void echo(char **cmd)
{
    int i ;
    int flag;
    int flag2;
    //fixed new line 
    i = 1;
    flag = 0;
    flag2 = 0;
    while(cmd[i])
    {
        if(!flag2 && cmd[i][0] == '-' && !check_nl(&cmd[i][1]))
        {
            flag = 1;
        }
        else
        {
            if(i > 1 && flag2)
                printf(" ");
            flag2 = 1;
            printf("%s", cmd[i]);
        }
        i++;
    }
    if(!flag)
        printf("\n");
}
char*    pwd(int i, t_list *envl)
{
    char *pwd ;
    int flag;

    flag = 0;
    pwd = getcwd(0, 0);
    if(!pwd)
    {
        pwd = fenv(envl, "PWD") + 4;
        flag = 1;
    }
    if (i == 1)
    {
        printf("%s\n", pwd);
        if(!flag)
            free(pwd);
    }
    return(pwd);
}
char    *fenv(t_list    *envl, char *str)
{
    while(envl)
    {
        if(ft_strnstr((char*)envl->content, str, ft_strlen(str)))
            return(ft_strdup(envl->content));
        envl = envl->next;
    }
    return(NULL);
}

int    cd(char **args, t_list **envl, t_list **exp_list)
{
    char *path;
    char *tmp;
    
    path = NULL;
    if(args[1] && args[2])
        return(write(2, "cd: string not is pwd\n", 23), 1);
    if (!args[1] || args[1][0] == '~')
    {
        path = fenv(*envl, "HOME=");
        if(!path)
            return(write(2, "cd: HOME not set", 17), 1);
        export(exp_list, envl, "OLDPWD=", tmp = pwd(0, *envl));
        free(tmp);
        chdir(path + 5);
    }   
    else if(args[1][0] == '-')
    {
        path = fenv(*envl, "OLDPWD=");
        export(exp_list, envl, "OLDPWD=", tmp = pwd(0, *envl));
        free(tmp);
        chdir(path + 7);
        pwd(1, *envl);
    }
    else
    {
        export(exp_list, envl, "OLDPWD=", tmp = pwd(0, *envl));
        free(tmp);
        if((tmp = getcwd(NULL, 0)) == NULL) 
            printf("cd: error retrieving current directory: getcwd: cannot access parent directories: No such file or directory\n");
        free(tmp);
        if(chdir(args[1]))
        {
            printf("cd: %s ", args[1]);
            error_func(errno, 1);
        }
    }
    export(exp_list, envl, "PWD=", tmp = pwd(0, *envl));
    free(tmp);
    //debug
    //printf("debug %s\n", fenv(*envl, "PWD"));
    //
    free(path);
    return(0);
}
void    print_env(t_list *envl)
{
    while(envl)
    {
        printf("%s\n", (char *)envl->content);
        envl = envl->next;
    }
}
t_list    *setup_env(char **env)
{
    t_list *envl;

    envl = NULL; 

    int i = 0;
    while(env[i])
    {
        ft_lstadd_back(&envl, ft_lstnew(ft_substr(env[i], 0, ft_strlen(env[i]))));
        i++;
    } 
    return(envl);
}
t_list  *setup_exp(t_list   *envl)
{
    t_list  *exp_list;
    char    *str;
    exp_list = NULL;
    while(envl)
    {
        str = (char*)envl->content;
        ft_lstadd_back(&exp_list, ft_lstnew(ft_substr(str, 0, ft_strlen(str))));
        envl = envl->next;
    }
    return(exp_list);
}
void print_export(t_list *exp_list)
{
    t_list  *head;
    t_list  *tmp;
    char    *temp;
    
    head = exp_list;
    while (exp_list->next)
    {
        tmp = exp_list->next;
        while(tmp)
        {
            if(ft_strncmp((char *)exp_list->content, (char *)tmp->content, ft_strlen((char *)exp_list->content)) > 0)
            {
                temp = (char *)exp_list->content;
                exp_list->content = tmp->content;
                tmp->content = temp;
            }
            tmp = tmp->next;
        }
        exp_list = exp_list->next;
    }
    while(head)
    {
        printf("%s\n", (char*)head->content);
        head = head->next;
    }
}
int find_var(t_list **list, char *var_name, char *var_value)
{
    char    *tmp;
    char    *str;
    int     flag;
    t_list *tmpl;

    flag = 0;
    tmpl = *list;
    while(tmpl)
    {
        str = (char*)tmpl->content;
        if(var_name && ft_strnstr(str, var_name, ft_strlen(var_name)))
        {
            flag = 1;
            tmp = str;
            tmpl->content = ft_strjoin(var_name, var_value);
            free(tmp);
        }
        tmpl = tmpl->next;
    }
    return(flag);
}
int check_var(char *var_name)
{
    int i ;
    
    i = 0;

    while(var_name[i+1])
    {
        if(var_name[i + 1] == '=' && var_name [i + 2] == '\0' && var_name[i] == '+')
        {
            return(0);
        }
        if(!ft_isalnum(var_name[i]) && var_name[i] != '_')
        {
            return (1);  
        }
        i++;
    }
    return(0);
}
int search_var_replace(t_list **list, char *var_name, char *var_value)
{
    char    *tmp;
    char    *tmp1;
    char    *str;
    t_list  *tmpl;
    int     flag;
// add second tmp for var_value when +=
    flag = 0;
    tmpl = *list;
    if(var_name && ft_strchr(var_name, '+'))
    {
        var_name = ft_strtrim(var_name, "+=");
        tmp = var_name;
        var_name = ft_strjoin(var_name, "=");
        free(tmp);
        flag = 1;
    }
    while(tmpl)
    {
        str = (char*)tmpl->content;
        // printf("*******************************(%s)\n", str);
        if(var_name && !var_value && ft_strnstr(str, var_name, ft_strlen(var_name)))
            return(1);
        else if(var_name && var_value && ft_strnstr(str, var_name, ft_strlen(var_name)) && flag)
        {
            tmp = str;
            tmp1 = var_value;
            var_value = ft_strjoin(ft_strchr(str, '=') + 1, var_value);
            tmpl->content = ft_strjoin(var_name, var_value);
            free(tmp);
            free(var_name);
            free(var_value);
            return(1);
        }
        else if(var_name && var_value && ft_strnstr(str, var_name, ft_strlen(var_name)) && !flag)
        {
            tmp = str;
            tmpl->content = ft_strjoin(var_name, var_value);
            free(tmp);
            return(1);
        }
        tmpl = tmpl->next;
    }
    // if(flag)
    return(0);
}

void export(t_list **exp_list, t_list**envl ,char *var_name, char *var_value)
{
    int     flag;
    int     flag2;
    int     free_var_name = 0; // flag to indicate whether to free var_name
    
    if(var_name && var_name[0] == '-')
    {
        write(2, "export: bad option: -s\n", 24);
        return ;
    }
    if(var_name && (!var_name[0] || ft_isdigit(var_name[0]) || (!ft_isalpha(var_name[0]) && var_name[0] != '_') || (check_var(var_name + 1))))
    {
        write(2, "export: not a valid identifier\n",32);
        return ;
    }
    flag = search_var_replace(envl, var_name, var_value);
    flag2 = search_var_replace(exp_list, var_name, var_value);
    if(var_name && ft_strchr(var_name, '+'))
    {
        var_name = ft_strtrim(var_name, "+=");
        char *tmp = var_name;
        var_name = ft_strjoin(var_name, "=");
        free(tmp);
        free_var_name = 1; // set flag to indicate var_name needs to be freed
    }
    if(!flag2 && !var_value && var_name)
        ft_lstadd_back(exp_list, ft_lstnew(ft_strjoin(var_name, "=")));
    if(!flag && var_name && var_value)
        ft_lstadd_back(envl, ft_lstnew(ft_strjoin(var_name, var_value)));
    if(!flag2 && var_name && var_value)
        ft_lstadd_back(exp_list, ft_lstnew(ft_strjoin(var_name, var_value)));
    if(!var_name)
        print_export(*exp_list);
    
    if (free_var_name)
        free(var_name); // free var_name if it contains '+'
    // free var_value if necessary
    // if (var_value)
    //     free(var_value);
}

//unset command
void unset(t_list **envl, char *var_name, int flag)
{
    t_list  *env;
    t_list  *prev;

    env = *envl;
    prev = NULL;
    var_name = ft_strjoin(var_name, "=");
    if(!var_name)
    {
        if(!flag)
            write(2, "unset: not enough arguments\n", 29);
        return ;
    }
    else if(check_var(var_name) && !flag)
    {
        if(!flag)
            write(2, "unset: invalid parameter name\n", 31);
        free(var_name);
        return ;
    }
    while(env)
    {
        if(!ft_strncmp((char*)env->content, var_name, ft_strlen(var_name)))
        {
            if(!prev)
                (*envl) = env->next;
            else
                prev->next = env->next;
            free(env->content);
            free(env);
            break;
        }
        prev = env;
        env = env->next;
    }
    free(var_name);
}

//Add some function
char **convert_to_array(t_list *envl)
{
    int i;
    char **cenv;
    t_list *tmp;
    
    i  = 0;
    tmp = envl;
    while(tmp)
    {
        i++;
        tmp = tmp->next;
    }
    cenv = (char **)malloc(sizeof(char *) * (i + 1));
    if(!cenv)
        return(0);
    i = 0;
    while (envl)
    {
        cenv[i++] = (char *)envl->content;
        envl = envl->next;
    }
    cenv[i] = 0;
    return(cenv);
}
void    ft_exit(t_token *head)
{
    int val;

    if(!head->args[1])
        exit(0);
    val = ft_atoi(head->args[1]);
    if((!val && head->args[1][0] != '0') || (!val && head->args[1][0] == '-'))
    {
        write(2, "bash: exit: numeric argument required\n", 39);
        exit(2);
    }
    else if (head->args[2])
    {
        write(2, "exit\nMinishell: exit: too many arguments\n", 42);
        return ;
    }
    else if (val != 0)
        exit(val % 256);

}

void init_export(t_token *head , t_list **envl, t_list **exp_list)
{
    int i;
    char *var_value;
    char *var_name;

    i = 1;
    while (head->args[i])
    {
        char *f ;
        f = NULL;
        if(head->args[i])
            f = ft_strchr(head->args[i], '=');       
        var_name = NULL;
        var_value = NULL;

        if(!f && head->args[i])
        {
            var_name = ft_substr(head->args[i], 0, ft_strlen(head->args[i]));
        }
        else if(f && head->args[i])
        {
            var_name =  ft_substr(head->args[i], 0, f - head->args[i] + 1 );
            var_value = ft_strdup(ft_strchr(head->args[i], '=') + 1);
        }
        export(exp_list,envl,var_name, var_value);
        free(var_name);
        free(var_value);
        i++;
    }
    return ;
}
