package net.cemetech.sfgp.scanline.geom;

import java.util.Collection;
import java.util.Iterator;
import java.util.NoSuchElementException;

import net.cemetech.sfgp.scanline.geom.Point.CoordName;

public class Edge implements Collection<Point>{
	public enum EndPoint{
		START, END;
		public int toNum() {
			switch(this){
				case START: return 0;
				case END: return 1;
				default: throw new IllegalArgumentException("bad enum value: " + this);
			}
		}
	}
	
	private Point[] coords;
	
	public Edge(Point start, Point end){
		coords = new Point[2];
		coords[EndPoint.START.toNum()] = start;
		coords[EndPoint.END.toNum()] = end;
	}
	
	public Edge(Point[] initCoords) {
		if(initCoords.length != 2) throw new IllegalArgumentException("An edge only has two points");
		coords = initCoords;
	}
	
	public Point getEndPoint(EndPoint e){ 
		return coords[e.toNum()];
	}

	public void setEndPoint(EndPoint e, Point newCoords){
		coords[e.toNum()] = newCoords;
	}
	
	public void flip(){
		Point tmp = coords[0];
		coords[0] = coords[1];
		coords[1] = tmp;
	}
	
	public static Edge flipped(Edge e){
		return new Edge(e.getEndPoint(EndPoint.END), e.getEndPoint(EndPoint.START));
	}
	
	// Same as the non-static version
	// However, here we require that the
	// starts already be aligned.
	public static int dot(Edge u, Edge v){
		Point us = u.getEndPoint(EndPoint.START);
		Point ue = u.getEndPoint(EndPoint.END);
		Point vs = v.getEndPoint(EndPoint.START);
		Point ve = v.getEndPoint(EndPoint.END);
		if(!us.equals(vs)){
			throw new IllegalArgumentException("We required the starts be aligned");
		}

		Point u0 = new Point(ue.getComponent(CoordName.X) - us.getComponent(CoordName.X),
							ue.getComponent(CoordName.Y) - us.getComponent(CoordName.Y),
							ue.getComponent(CoordName.Z) - us.getComponent(CoordName.Z));
		Point v0 = new Point(ve.getComponent(CoordName.X) - vs.getComponent(CoordName.X),
							ve.getComponent(CoordName.Y) - vs.getComponent(CoordName.Y),
							ve.getComponent(CoordName.Z) - vs.getComponent(CoordName.Z));
		
		
		return		u0.getComponent(CoordName.X) * v0.getComponent(CoordName.X)
				+	u0.getComponent(CoordName.Y) * v0.getComponent(CoordName.Y)
				+	u0.getComponent(CoordName.Z) * v0.getComponent(CoordName.Z);
	}
	
	// Dot product of the vectors found by translating two edges 
	// with a shared vertex to the origin.
	public int dot(Edge e2){
		Edge d1 = null, d2 = null;
		for(Point p1 : this){
			for(Point p2 : e2){
				if(p1.equals(p2)){
					d1 = (p1 == this.getEndPoint(EndPoint.START)) ? this : Edge.flipped(this);
					d2 = (p2 == e2.getEndPoint(EndPoint.START)) ? e2 : Edge.flipped(e2);
					break;
				}
			}
		}
		if(d1 == null || d2 == null){
			throw new IllegalArgumentException("We required that at least one endpoint be shared");
		}
		return dot(d1, d2);
	}

	@Override
	public Iterator<Point> iterator() {
		return new Iterator<Point>(){
			int i = 0;

			@Override
			public boolean hasNext() {
				return (i != coords.length);
			}

			@Override
			public Point next() {
				if(i == coords.length){
					throw new NoSuchElementException();
				} else {
					return coords[i++];
				}
			}

			@Override
			public void remove() {
				throw new UnsupportedOperationException("can't modify edges like this");
			}
			
		};
	}

	@Override
	public int size() {
		return 2;
	}

	@Override
	public boolean isEmpty() {
		return false;
	}

	@Override
	public boolean contains(Object o) {
		boolean ret = false;
		if(o instanceof Point){
			for(int i = 0; i < coords.length; i++) ret |= ((Point)o).equals(coords[i]);
		}
		return ret;
	}

	@Override
	public Object[] toArray() {
		return toArray(new Integer[coords.length]);
	}

	@Override
	public <T> T[] toArray(T[] a) {
		if(a.length < coords.length) a = (T[])(new Point[coords.length]);
		if(a.length > coords.length) a[coords.length] = null;
		System.arraycopy(coords, 0, a, 0, coords.length);
		return a;
	}

	@Override
	public boolean add(Point e) {
		throw new UnsupportedOperationException("can't modify edges like this");
	}

	@Override
	public boolean remove(Object o) {
		throw new UnsupportedOperationException("can't modify edges like this");
	}

	@Override
	public boolean containsAll(Collection<?> c) {
		boolean ret = true;
		for(Object o : c){
			ret &= contains(o);
			if(!ret) break;
		}
		return ret;
	}

	@Override
	public boolean addAll(Collection<? extends Point> c) {
		throw new UnsupportedOperationException("can't modify edges like this");
	}

	@Override
	public boolean removeAll(Collection<?> c) {
		throw new UnsupportedOperationException("can't modify edges like this");
	}

	@Override
	public boolean retainAll(Collection<?> c) {
		throw new UnsupportedOperationException("can't modify edges like this");
	}

	@Override
	public void clear() {
		throw new UnsupportedOperationException("can't modify edges like this");
	}
	
	public boolean equals(Object o){
		boolean ret = false;
		if(o instanceof Edge){
			ret = true;
			for(int i = 0; i < coords.length; i++) ret &= ((Edge)o).coords[i] == coords[i];
		}
		return ret;
	}
	
	public int hashCode(){
		int a = 0;
		for(Point p : coords) a ^= p.hashCode();
		return a;
	}
}
