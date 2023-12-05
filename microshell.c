/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   microshell.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aperrein <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/06 09:30:33 by aperrein          #+#    #+#             */
/*   Updated: 2023/07/06 10:29:31 by aperrein         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

int	erreur(char *s, char *arg)
{
	int	i;

	i = 0;
	while (s[i])
		write(2, &s[i++], 1);
	i = 0;
	if (arg)
	{
		while (arg[i])
			write(2, &arg[i++], 1);
	}
	write(2, "\n", 1);
	return (1);
}

int	exec(char *s[], int i, int tmp_fd, char *env[])
{
	s[i] = 0;
	dup2(tmp_fd, 0);
	close(tmp_fd);
	execve(s[0], s, env);
	return (erreur("error: cannot execute ", s[0]));
}

int	main(int argc, char *s[], char *env[])
{
	int	i;
	int	fd[2];
	int	tmp_fd;

	(void)argc;
	i = 0;
	tmp_fd = dup(0);
	while (s[i] && s[i +1])
	{
		s = &s[i +1];
		i = 0;
		while (s[i] && strcmp(s[i], ";") && strcmp(s[i], "|"))
			i++;
		if (strcmp(s[0], "cd") == 0)
		{
			if (i !=2)
				erreur("error: cd: bad arguments", 0);
			else if (chdir(s[1]))
				erreur("error: cd: cannot change directory to ", s[1]);
		}
		else if (i != 0)
		{
			pipe(fd);
			if (fork() == 0)
			{
				close(fd[0]);
				if (s[i] && strcmp(s[i], "|") == 0)
					dup2(fd[1], 1);
				if (exec(s, i, tmp_fd, env))
					return (1);
			}
			else
			{
				waitpid(-1, 0, 0);
				close(fd[1]);
				close(tmp_fd);
				if (s[i] && strcmp(s[i], "|") == 0)
					tmp_fd = fd[0];
				else
					tmp_fd = dup(0);
			}
		}
	}
	close(tmp_fd);
	return (0);
}
