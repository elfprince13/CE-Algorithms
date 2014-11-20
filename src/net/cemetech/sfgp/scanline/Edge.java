package net.cemetech.sfgp.scanline;

import java.util.Collection;
import java.util.Iterator;
import java.util.NoSuchElementException;

import net.cemetech.sfgp.scanline.Point.CoordName;

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
			for(int i = 0; i < coords.length; i++) ret |= ((Point)o) == coords[i];
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
