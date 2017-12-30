package gra;

import java.io.IOException;
import java.util.*;

public class Digraph<V>
{

    public static class Edge<V>
    {
        private V vertex;

        public Edge(V v)
        {
            vertex = v;
        }

        public V getVertex()
        {
            return vertex;
        }

        @Override
        public String toString()
        {
            return "Edge [vertex=" + vertex + "]";
        }

    }

    private Map<V, List<Edge<V>>> neighbors = new HashMap<V, List<Edge<V>>>();

    private int nr_edges;

    public String toString()
    {
        StringBuffer s = new StringBuffer();
        for (V v : neighbors.keySet())
            s.append("\n    " + v + " -> " + neighbors.get(v));
        return s.toString();
    }

    public void add(V vertex)
    {
        if (neighbors.containsKey(vertex))
            return;
        neighbors.put(vertex, new ArrayList<Edge<V>>());
    }

    public int getNumberOfEdges()
    {
        int sum = 0;
        for(List<Edge<V>> outBounds : neighbors.values())
        {
            sum += outBounds.size();
        }
        return sum;
    }

    public boolean contains(V vertex)
    {
        return neighbors.containsKey(vertex);
    }

    public void reset(V vertex)
    {
        neighbors.put(vertex, new ArrayList<Edge<V>>());
    }

    public void add(V from, V to)
    {
        this.add(from);
        this.add(to);
        neighbors.get(from).add(new Edge<V>(to));
    }

    public int outDegree(int vertex)
    {
        return neighbors.get(vertex).size();
    }

    public int inDegree(V vertex)
    {
        return inboundNeighbors(vertex).size();
    }

    public List<V> outboundNeighbors(V vertex)
    {
        List<V> list = new ArrayList<V>();
        for(Edge<V> e: neighbors.get(vertex))
            list.add(e.vertex);
        return list;
    }

    public List<V> inboundNeighbors(V inboundVertex)
    {
        List<V> inList = new ArrayList<V>();
        for (V to : neighbors.keySet())
        {
            for (Edge e : neighbors.get(to))
                if (e.vertex.equals(inboundVertex))
                    inList.add(to);
        }
        return inList;
    }

    public boolean isCyclicRec()
    {
        for (V node : neighbors.keySet())
        {
            Set<V> initPath = new HashSet<>();
            if (isCyclicRec(node, initPath))
            {
                return true;
            }
        }
        return false;
    }

    private boolean isCyclicRec(V currNode, Set<V> path)
    {
        if (path.contains(currNode))
        {
            return true;
        }
        else
        {
            path.add(currNode);
            for (Edge<V> node : neighbors.get(currNode))
            {
                if (isCyclicRec(node.getVertex(), path))
                {
                    return true;
                }
                else
                {
                    path.remove(node);
                }
            }
        }
        return false;
    }
}
