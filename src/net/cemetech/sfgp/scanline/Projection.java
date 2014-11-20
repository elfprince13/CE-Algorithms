package net.cemetech.sfgp.scanline;

import java.util.LinkedList;

public abstract class Projection {
	public static Projection identity(){
		return new Projection(){
			public Point projectPoint(Point p){ return p; }
		};
	}
	
	abstract public Point projectPoint(Point p);
	public Edge projectEdge(Edge e){
		Point[] projected = new Point[2];
		int i = 0;
		for(Point p : e){
			projected[i++] = projectPoint(p);
		}
		return new Edge(projected);
	}
	
	public Primitive projectPrimitive(Primitive p){
		LinkedList<Edge> projected = new LinkedList<Edge>();
		for(Edge e : p.boundary){
			projected.addLast(projectEdge(e));
		}
		Primitive outP = new Primitive(projected);
		outP.color = p.color;
		return outP;
	}
}
