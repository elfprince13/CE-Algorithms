package net.cemetech.sfgp.scanline.geom;

import java.awt.Color;
import java.util.LinkedList;

import net.cemetech.sfgp.scanline.geom.Edge.EndPoint;
import net.cemetech.sfgp.scanline.geom.Point.CoordName;

public class Primitive implements Comparable<Primitive> {
	public LinkedList<Edge> boundary;
	public Color color;
	public int unique;
	
	public static int uniqueV = 0;
	
	public Primitive(LinkedList<Edge> b){
		boundary = b;
		color = Color.black;
		unique = uniqueV++;
	}
	
	public static Primitive makeLine(Edge e){
		LinkedList<Edge> le = new LinkedList<Edge>();
		le.add(e);
		return new Primitive(le);
	}
	
	public static Primitive makeTri(Edge e1, Edge e2, Edge e3){
		LinkedList<Edge> le = new LinkedList<Edge>();
		le.add(e1);
		le.add(e2);
		le.add(e3);
		return new Primitive(le);
	}
	
	public static Primitive makeQuad(Edge e1, Edge e2, Edge e3, Edge e4){
		LinkedList<Edge> le = new LinkedList<Edge>();
		le.add(e1);
		le.add(e2);
		le.add(e3);
		le.add(e4);
		return new Primitive(le);
	}
	
	public int getArity(){ return boundary.size(); }
	
	public int getZForXY(int x, int y){
		assert(getArity() > 0);
		if(getArity() == 1){
			// Alpha blend! wut wut.
			Edge v = boundary.getFirst();
			Point vs = v.getEndPoint(EndPoint.START);
			Point ve = v.getEndPoint(EndPoint.END);
			
			int sx = vs.getComponent(CoordName.X);
			int sy = vs.getComponent(CoordName.Y);
			int sz = vs.getComponent(CoordName.Z);
			
			int dx = ve.getComponent(CoordName.X) - sx;
			int dy = ve.getComponent(CoordName.Y) - sy;
			int dz = ve.getComponent(CoordName.Z) - sz;
			
			assert((dx == 0 && dy == 0) || (dx != 0 && dy != 0));
			
			// Theoretically these should be the same, but
			// we'll average for robustness
			int xNumer = dz * (x - sx);
			int yNumer = dz * (y - sy);
			int xEst = (dx == 0) ? Math.min(0,  dz) : (xNumer / dx);
			int yEst = (dy == 0) ? Math.min(0,  dz) : (yNumer / dy);
			
			return sz + (xEst + yEst) / 2; 
		} else {
			Edge e1 = boundary.getFirst();
			Edge e2 = boundary.get(1);
			
			Point us = e1.getEndPoint(EndPoint.START);
			Point ue = e1.getEndPoint(EndPoint.END);
			Point u = new Point(
					us.getComponent(CoordName.X) - ue.getComponent(CoordName.X),
					us.getComponent(CoordName.Y) - ue.getComponent(CoordName.Y),
					us.getComponent(CoordName.Z) - ue.getComponent(CoordName.Z));
			int ux = u.getComponent(CoordName.X);
			int uy = u.getComponent(CoordName.Y);
			int uz = u.getComponent(CoordName.Z);
			
			Point vs = e2.getEndPoint(EndPoint.START);
			Point ve = e2.getEndPoint(EndPoint.END);
			Point v = new Point(
					vs.getComponent(CoordName.X) - ve.getComponent(CoordName.X),
					vs.getComponent(CoordName.Y) - ve.getComponent(CoordName.Y),
					vs.getComponent(CoordName.Z) - ve.getComponent(CoordName.Z));
			int vx = v.getComponent(CoordName.X);
			int vy = v.getComponent(CoordName.Y);
			int vz = v.getComponent(CoordName.Z);
			
			int nx = uy * vz - uz * vy;
			int ny = uz * vx - ux * vz;
			int nz = ux * vy - uy * vx;
			
			int d = -nx * us.getComponent(CoordName.X)
					-ny * us.getComponent(CoordName.Y)
					-nz * us.getComponent(CoordName.Z);
			
			int numer = (-d - nx * x - ny * y);
			
			return (nz == 0) ? ((numer > 0) ? Integer.MAX_VALUE : Integer.MIN_VALUE) : (numer / nz) ;
		}
	
	}
	//*
	public int hashCode() {
		int ret = 0;
		for(Edge e : boundary){
			ret ^= e.hashCode();
		}
		ret ^= color.hashCode();
		return ret;
	}
	//*/
	public int compareTo(Primitive o){
		return this.unique - o.unique;
	}

}
