package net.cemetech.sfgp.scanline;

import java.awt.Color;
import java.util.LinkedList;

public class Primitive {
	public LinkedList<Edge> boundary;
	public Color color;
	
	public Primitive(LinkedList<Edge> b){
		boundary = b;
		color = Color.black;
	}
	
	static Primitive makeLine(Edge e){
		LinkedList<Edge> le = new LinkedList<Edge>();
		le.add(e);
		return new Primitive(le);
	}
	
	static Primitive makeTri(Edge e1, Edge e2, Edge e3){
		LinkedList<Edge> le = new LinkedList<Edge>();
		le.add(e1);
		le.add(e2);
		le.add(e3);
		return new Primitive(le);
	}
	
	static Primitive makeQuad(Edge e1, Edge e2, Edge e3, Edge e4){
		LinkedList<Edge> le = new LinkedList<Edge>();
		le.add(e1);
		le.add(e2);
		le.add(e3);
		le.add(e4);
		return new Primitive(le);
	}
	
	public int getArity(){ return boundary.size(); }
	
	public int getZForXY(int x, int y){
		if(getArity() == 1){
			return 0; 
		} else {
			Edge e1 = boundary.getFirst();
			Edge e2 = boundary.get(1);
			
			Point us = e1.getEndPoint(Edge.EndPoint.START);
			Point ue = e1.getEndPoint(Edge.EndPoint.END);
			Point u = new Point(
					us.getComponent(Point.CoordName.X) - ue.getComponent(Point.CoordName.X),
					us.getComponent(Point.CoordName.Y) - ue.getComponent(Point.CoordName.Y),
					us.getComponent(Point.CoordName.Z) - ue.getComponent(Point.CoordName.Z));
			int ux = u.getComponent(Point.CoordName.X);
			int uy = u.getComponent(Point.CoordName.Y);
			int uz = u.getComponent(Point.CoordName.Z);
			
			Point vs = e2.getEndPoint(Edge.EndPoint.START);
			Point ve = e2.getEndPoint(Edge.EndPoint.END);
			Point v = new Point(
					vs.getComponent(Point.CoordName.X) - ve.getComponent(Point.CoordName.X),
					vs.getComponent(Point.CoordName.Y) - ve.getComponent(Point.CoordName.Y),
					vs.getComponent(Point.CoordName.Z) - ve.getComponent(Point.CoordName.Z));
			int vx = v.getComponent(Point.CoordName.X);
			int vy = v.getComponent(Point.CoordName.Y);
			int vz = v.getComponent(Point.CoordName.Z);
			
			int nx = uy * vz - uz * vy;
			int ny = uz * vx - ux * vz;
			int nz = ux * vy - uy * vx;
			
			int d = -nx * us.getComponent(Point.CoordName.X)
					-ny * us.getComponent(Point.CoordName.Y)
					-nz * us.getComponent(Point.CoordName.Z);
			
			int numer = (-d - nx * x - ny * y);
			
			return (nz == 0) ? ((numer > 0) ? Integer.MAX_VALUE : Integer.MIN_VALUE) : (numer / nz) ;
		}
	
	}

}
