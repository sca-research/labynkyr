**Repository configuration**

We have two repositories for labynkyr:

 1.  `labynkyr-private`
 2.  `labynkyr`

The latter is public.

The public repository contains only public facing release code.  We should do all development on master or any other branch, and push changes to labynkyr-private.  This keeps all our development work private using standard workflows.

If we want to make a set of changes public, the process is to manually copy the changes across to the public repository. The public repository will then essentially contain snapshot-like sections of the main branches at various points in time.  This is a pretty manual process, but we shouldn't need to do it very often at all, and it helps against accidental pushes to a public branch.  If anyone has a better idea, please let me know.
