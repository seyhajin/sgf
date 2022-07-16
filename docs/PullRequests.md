
#### Forking SGF and making Pull Requests:

1. Create a github account and login.
  

2. Search for the SGF repository: blitz-research/sgf (make sure to 'search all github').
   

3. Click 'Fork' at top right of the SGF page. You might also want to 'Watch' and/or 'Star' the repository, also at top right.
  

4. Click 'Create fork' button. You can change the repos name/description here if you want too, but IMO leave these as-is.
  

5. This will create a copy of the blitz-research SGF repository identical to the original except owned by you so only you can make changes to it.
  

6. If the blitz-research SGF repository changes, you can 'catch-up' with those changes by clicking 'Fetch upstream/Fetch and Merge'. You can also check what changes this will result in by clicking 'Fetch upstream/Compare'.
  

7. You can now clone the repository to your local computer, but before you do this you should create a 'Personal access token'. Without this, you will only have read access to the repository and won't be able to 'push' any local changes back to the github repository.
  

8. Go to Settings -> Developer settings-> Personal access tokens -> Generate new token. Fill in token details and click on 'Generate Token' - here's what I used for details:
   ~~~
   Note: Repository access  
   Expiration: never  
   Scopes: repo
   ~~~
   This will create a token that looks something like this:
   ~~~
   ghp_f4FChGQ1vxQtUVblahblahblahHqbf1YIf4Z
   ~~~
   This is effectively a password you can use in combination with your username to give you full access to your repositories at github, BUT I was not able to get this to work easily! Lots of tutorials around claim that git will ask you for username and password at some point but this didn't happen for me while writing this tutorial. Instead, I had to add username/password to the github repository URL.
  

9. Clone the repository, including your github username and password (ie: personal access token) in the URL, like this:
    ~~~
    $ git clone https://myusername:ghp_f4FChGblahblahblahJR0frmRaHqbf1YIf4Z@github.com/myusername/sgf.git
    ~~~
   
10. You should now be able to make changes to the local repository and 'push' them back to github, eg:
    ~~~
    $ cd sgf
    $ <edit some files>
    $ git add -u
    $ git commit -m "Make some changes."
    $ git push
    ~~~

After experimenting a bit more, it turns out there's a git configuration variable called 'confguration.helper' that affects how git looks for usernames and passwords, and therefore wether or not the user is prompted for them. However, I think the above method of including username/password in the git url is probably the most bullet proof way to go.
