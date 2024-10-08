#include <bits/stdc++.h>
using namespace std;

#define rev(ans) reverse(ans.begin(), ans.end())
#define pb emplace_back
#define fst first
#define scd second

// At each node do a level order traversal and check if it forms a cycle i.e it can be reached 

class Solution {
public:
    int n,res=INT_MAX;
    void bfs(int st,vector<vector<int>>& adj,vector<int>& visit)
    {
    	queue<int> q;
    	q.push(st);
    	int lvl=0;
    	while(!q.empty())
    	{
    		int sz=q.size();

    		for(int i=0;i<sz;i++)
    		{
    			int f=q.front();
    			q.pop();
    			visit[f]=true;
    			for(auto nei:adj[f])
    			{
    				if(nei!=f && nei==st) return lvl;
    				if(!visit[nei]) q.push(nei);
    			}
    		}
    		lvl++;
    	}
    }
    int findShortestCycle(int n, vector<vector<int>>& edges) {
        this->n=n;
        vector<vector<int>> adj(n);
        vector<bool> visit(n);

        for(auto& e:edges)
        {
        	int x=e[0],y=e[1];
            adj[x].pb(y);
            adj[y].pb(x);
        }
        for(int i=0;i<n;i++)
        {
        	visit.assign(n,false);
        	bfs(adj,visit);
        }
        return res==INT_MAX?-1:res;
    }
};
int main()
{
	int n;
	cin>>n;
}