using UnityEngine;
using System.Collections;

public class Rigid_Bunny : MonoBehaviour
{
    private const float dt = 0.015f;
    private const float linear_decay = 0.999f; // for velocity decay
    private const float angular_decay = 0.98f;
    private const float un = 0.3f; // spring factor  [0,1]
    private const float ut = 0.5f; // friction factor
    private const float min_velocity = 0.1f;

    private readonly Vector3 g = new Vector3(0, -9.8f, 0);

    private float restitution = 0.5f;  // for collision
    private bool launched = false;
    private Vector3 v = new Vector3(0, 0, 0);   // velocity
    private Vector3 w = new Vector3(0, 0, 0);   // angular velocity

    private float mass;  // mass
    private Matrix4x4 I_ref; // reference inertia
    private Matrix4x4 identity_m;

    // Use this for initialization
    private void Start()
    {
        Mesh mesh = GetComponent<MeshFilter>().mesh;
        Vector3[] vertices = mesh.vertices;

        float m = 1;
        mass = 0;
        for (int i = 0; i < vertices.Length; i++)
        {
            mass += m;
            float diag = m * vertices[i].sqrMagnitude;
            I_ref[0, 0] += diag;
            I_ref[1, 1] += diag;
            I_ref[2, 2] += diag;
            I_ref[0, 0] -= m * vertices[i][0] * vertices[i][0];
            I_ref[0, 1] -= m * vertices[i][0] * vertices[i][1];
            I_ref[0, 2] -= m * vertices[i][0] * vertices[i][2];
            I_ref[1, 0] -= m * vertices[i][1] * vertices[i][0];
            I_ref[1, 1] -= m * vertices[i][1] * vertices[i][1];
            I_ref[1, 2] -= m * vertices[i][1] * vertices[i][2];
            I_ref[2, 0] -= m * vertices[i][2] * vertices[i][0];
            I_ref[2, 1] -= m * vertices[i][2] * vertices[i][1];
            I_ref[2, 2] -= m * vertices[i][2] * vertices[i][2];
        }
        I_ref[3, 3] = 1;

        float i_m = 1 / mass;
        identity_m[0, 0] = i_m;
        identity_m[1, 1] = i_m;
        identity_m[2, 2] = i_m;
        identity_m[3, 3] = i_m;
    }

    private Matrix4x4 Get_Cross_Matrix(Vector3 a)
    {
        //Get the cross product matrix of vector a
        Matrix4x4 A = Matrix4x4.zero;
        A[0, 0] = 0;
        A[0, 1] = -a[2];
        A[0, 2] = a[1];
        A[1, 0] = a[2];
        A[1, 1] = 0;
        A[1, 2] = -a[0];
        A[2, 0] = -a[1];
        A[2, 1] = a[0];
        A[2, 2] = 0;
        A[3, 3] = 1;
        return A;
    }

    private Matrix4x4 Sub_Matrix(Matrix4x4 a, Matrix4x4 b)
    {
        Matrix4x4 res = Matrix4x4.zero;
        for (int i = 0; i < 4; i++)
        {
            Vector4 column = a.GetColumn(i) - b.GetColumn(i);
            res.SetColumn(i, column);
        }
        return res;
    }

    private Quaternion Add_Quaternion(Quaternion a, Quaternion b)
    {
        return new Quaternion(a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w);
    }

    // In this function, update v and w by the impulse due to the collision with
    //a plane <P, N>
    private void Collision_Impulse(Vector3 P, Vector3 N)
    {
        Vector3 x = transform.position;
        Quaternion q = transform.rotation;
        Matrix4x4 R = Matrix4x4.Rotate(q);
        Matrix4x4 I = R * I_ref * R.transpose;

        Mesh mesh = GetComponent<MeshFilter>().mesh;
        Vector3[] vertices = mesh.vertices;

        Vector3 vertices_all = Vector3.zero;
        int collisionCount = 0;
        for (int i = 0; i < vertices.Length; i++)
        {
            Vector3 x_i = x + R.MultiplyPoint(vertices[i]);
            bool isCollision = Vector3.Dot(x_i - P, N) < 0;
            if (!isCollision) { continue; }

            vertices_all += vertices[i];
            collisionCount++;
        }

        if (collisionCount == 0) { return; }

        Vector3 Rr_i = R.MultiplyPoint(vertices_all / collisionCount);
        Vector3 v_i = v + Vector3.Cross(w, Rr_i);
        bool isV = Vector3.Dot(v_i, N) < 0;
        if (!isV) { return; }

        Vector3 vn_i = Vector3.Dot(v_i, N) * N;
        Vector3 vt_i = v_i - vn_i;

        vn_i = vn_i.magnitude < min_velocity ? Vector3.zero : vn_i;
        vt_i = vt_i.magnitude < min_velocity ? Vector3.zero : vt_i;

        float a = Mathf.Max(1 - ut * (1 + un) * vn_i.magnitude / vt_i.magnitude, 0);

        Vector3 vn_new_i = -un * vn_i;
        Vector3 vt_new_i = a * vt_i;

        Vector3 v_new_i = vn_new_i + vt_new_i;

        Matrix4x4 Rr_i_x = Get_Cross_Matrix(Rr_i);
        Matrix4x4 K = Sub_Matrix(identity_m, Rr_i_x * I.inverse * Rr_i_x);

        Vector3 j = K.inverse.MultiplyVector(v_new_i - v_i);

        v += j / mass;
        w += I.inverse.MultiplyVector(Vector3.Cross(Rr_i, j));
    }

    // Update is called once per frame
    private void Update()
    {
        //Game Control
        if (Input.GetKey("r"))
        {
            transform.position = new Vector3(0, 0.6f, 0);
            restitution = 0.5f;
            launched = false;
        }
        if (Input.GetKey("l"))
        {
            v = new Vector3(5, 2, 0);
            launched = true;
        }

        if (!launched)
        {
            return;
        }

        // Part I: Update velocities
        v += g * dt;
        v *= linear_decay;

        w *= angular_decay;

        // Part II: Collision Impulse
        Collision_Impulse(new Vector3(0, 0.01f, 0), new Vector3(0, 1, 0));
        Collision_Impulse(new Vector3(2, 0, 0), new Vector3(-1, 0, 0));

        // Part III: Update position & orientation
        //Update linear status
        Vector3 x = transform.position;
        //Update angular status
        Quaternion q = transform.rotation;

        x = x + dt * v;
        Quaternion dtw_2 = new Quaternion(dt / 2 * w.x, dt / 2 * w.y, dt / 2 * w.z, 0);
        q = Add_Quaternion(q, dtw_2 * q).normalized;

        // Part IV: Assign to the object
        transform.position = x;
        transform.rotation = q;

        if (w.magnitude < min_velocity || v.magnitude < min_velocity)
        {
            restitution -= 0.01f;
            if (restitution < 0)
            {
                launched = false;
            }
        }
    }
}
